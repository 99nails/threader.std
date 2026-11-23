#include <gtest/gtest.h>

#include "HandlerSslSocket.h"
#include "SocketUtils.h"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <string>
#include <thread>

using namespace Threader::Threads;
using namespace std::chrono_literals;

namespace
{

// Наследник с доступом к защищённым методам хендшейка
class TestableSslSocket : public HandlerSslSocket
{
public:
    using HandlerSslSocket::HandlerSslSocket;

    SSL *instance() { return sslInstance(); }
    bool attachDescriptor() { return setInstanceDescriptor(); }
    int clientHandshake() { return connect(); }   // SSL_connect
    int serverHandshake() { return accept(); }    // SSL_accept

    void forceState(HandlerBase::ConnectionState state)
     {
         setConnectionState(state);
     }
};

int makeSocketPair(Descriptor &clientSide, Descriptor &serverSide)
{
    int fds[2];
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0)
        return -1;
    clientSide = fds[0];
    serverSide = fds[1];
    return 0;
}

std::string lastOpensslError()
{
    char buffer[256];
    ::ERR_error_string_n(::ERR_get_error(), buffer, sizeof(buffer));
    return buffer;
}

// Чтение до expected байт с таймаутом; неблокирующий SSL_read даёт WANT_READ
std::string readAvailable(HandlerSslSocket &socket, size_t expected,
                          const std::chrono::milliseconds &timeout)
{
    std::string result;
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (result.size() < expected
           && std::chrono::steady_clock::now() < deadline)
    {
        char buffer[256];
        int received = socket.read(buffer, sizeof(buffer));
        if (received > 0)
            result.append(buffer, static_cast<size_t>(received));
        else
            std::this_thread::sleep_for(1ms);
    }
    return result;
}

} // namespace

// Полный TLS-хендшейк клиент<->сервер поверх socketpair:
// клиент повторяет ветку Connecting из HandlerSslSocket::process(),
// сервер — ветку Connecting из assign().
TEST(SslHandshake, FullHandshakeAndEchoOverSocketPair)
{
    ::alarm(60); // предохранитель: зависший хендшейк не подвесит раннер молча

    Descriptor clientFd = INVALID_DESCRIPTOR, serverFd = INVALID_DESCRIPTOR;
    ASSERT_EQ(makeSocketPair(clientFd, serverFd), 0);

    constexpr uint16_t kPort = 443; // только метаданные устройства
    constexpr char kHost[] = "127.0.0.1";

    // Клиент: невалидный дескриптор => TcpSocketType::Client
    TestableSslSocket client(nullptr, kHost, kPort);
    client.forceState(HandlerBase::ConnectionState::Connecting);
    client.setDescriptor(clientFd);          // останется Connecting
    ASSERT_TRUE(client.instance());          // SSL от клиентского контекста
    ASSERT_TRUE(client.attachDescriptor());  // SSL_set_fd
    SocketUtils::setBlockingMode(clientFd, false);

    // Сервер: валидный дескриптор => TcpSocketType::Server, Connecting
    TestableSslSocket server(nullptr, kHost, kPort, serverFd);
    ASSERT_TRUE(server.instance());          // SSL от серверного контекста
    ASSERT_TRUE(server.attachDescriptor());
    SocketUtils::setBlockingMode(serverFd, false);

    // Пошаговый неблокирующий хендшейк: SSL_connect <-> SSL_accept
    int clientRet = 0, serverRet = 0;
    for (int attempt = 0; attempt < 10000
         && (clientRet != 1 || serverRet != 1); ++attempt)
    {
        if (clientRet != 1)
        {
            clientRet = client.clientHandshake();
            if (clientRet != 1)
            {
                auto err = SSL_get_error(client.instance(), clientRet);
                ASSERT_TRUE(err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
                    << "клиент: " << lastOpensslError();
            }
        }

        if (serverRet != 1)
        {
            serverRet = server.serverHandshake();
            if (serverRet != 1)
            {
                auto err = SSL_get_error(server.instance(), serverRet);
                ASSERT_TRUE(err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
                    << "сервер: " << lastOpensslError();
            }
        }

        if (clientRet == 1 && serverRet == 1)
            break;
        usleep(1000); // 10 с максимум на весь хендшейк
    }

    ASSERT_EQ(clientRet, 1) << "хендшейк клиента не завершён";
    ASSERT_EQ(serverRet, 1) << "хендшейк сервера не завершён";

    client.forceState(HandlerBase::ConnectionState::Connected);
    server.forceState(HandlerBase::ConnectionState::Connected);

    // Эхо-обмен зашифрованными данными в обе стороны
    const std::string payload = "hello over tls";
    ASSERT_EQ(client.write(payload.data(), int(payload.size())), int(payload.size()));

    const auto echoed = readAvailable(server, payload.size(), 5000ms);
    ASSERT_EQ(echoed, payload);

    ASSERT_EQ(server.write(payload.data(), int(payload.size())), int(payload.size()));
    const auto received = readAvailable(client, payload.size(), 5000ms);
    EXPECT_EQ(received, payload);

    // Метаданные TLS: до хендшейка там было "(NONE)", теперь — реальный шифр
    EXPECT_STRNE(client.getCipherName().c_str(), "(NONE)");
    EXPECT_STRNE(client.getCipherVersion().c_str(), "(NONE)");
    EXPECT_STRNE(server.getCipherName().c_str(), "(NONE)");

    const std::string version = SSL_get_version(client.instance());
    EXPECT_TRUE(version.rfind("TLSv1.", 0) == 0) << "версия: " << version;

    // Сервер должен предъявить встроенный самоподписанный сертификат
    X509 *peer = SSL_get_peer_certificate(client.instance());
    ASSERT_NE(peer, nullptr) << "сервер не предъявил сертификат";
    char cn[256] = {};
    X509_NAME_get_text_by_NID(X509_get_subject_name(peer),
                              NID_commonName, cn, sizeof(cn));
    X509_free(peer);
    EXPECT_STREQ(cn, "systemt.ru") << "CN сертификата: " << cn;

    ::alarm(0);
}
