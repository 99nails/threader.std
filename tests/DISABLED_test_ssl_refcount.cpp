#include <gtest/gtest.h>

#include "HandlerSslSocket.h"
#include "SocketUtils.h"

#include <atomic>
#include <openssl/ssl.h>
#include <thread>
#include <vector>

using namespace Threader::Threads;


namespace
{

// Наследник для доступа к protected-членам
class TestableSslSocket : public HandlerSslSocket
{
public:
    using HandlerSslSocket::HandlerSslSocket;

    SSL *instance() { return sslInstance(); }

    SSL_CTX *clientContext() { return sslContext(); }

    SSL_CTX *serverContext() { return sslContext(); }
};

// Валидный дескриптор для серверного сокета без сети
Descriptor makeServerDescriptor()
{
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0)
        return INVALID_DESCRIPTOR;
    ::close(fds[1]); // одноразовый peer
    return fds[0];
}

constexpr const char *kTestHost = "127.0.0.1";
constexpr uint16_t kTestPort = 443;

} // namespace

// BUG-3: двойной декремент счётчика клиентского контекста.
// Схема: два клиента, у каждого создан SSL-объект; close+destroy первого.
// close(): 2 -> 1; деструктор: 1 -> 0 -> SSL_CTX_free — контекст освобождён,
// а sslB остался привязан к нему.
// До исправления: UB (ловится ASAN либо падением на SSL_get_version).
// После исправления: тест обязан проходить.
TEST(DISABLED_SslRefcount, DestroyingOneClientFreesContextOfAnother)
{
    SSL *sslB = nullptr;
    {
        TestableSslSocket a(nullptr, "127.0.0.1", 443);
        TestableSslSocket b(nullptr, "127.0.0.1", 443);

        sslB = b.instance();
        ASSERT_NE(sslB, nullptr);

        a.close();
    }

    // это обращение к освободившемуся контексту — до исправления UB
    EXPECT_NE(SSL_get_version(sslB), nullptr);
}

TEST(DISABLED_SslRefcount, DestroyingOneClientFreesContextOfAnotherExtended)
{
    // два клиента: конструкторы дали refcount = 2, контекст общий
    TestableSslSocket a(nullptr, "127.0.0.1", 443);
    SSL_CTX *ctxBefore = nullptr;
    {
        TestableSslSocket b(nullptr, "127.0.0.1", 443);
        ASSERT_NE(b.instance(), nullptr); // создаёт SSL от общего контекста
        ctxBefore = b.clientContext();
        ASSERT_NE(ctxBefore, nullptr);

        a.close(); // refcount 2 -> 1
    }   // деструктор b: при баге refcount -> 0, контекст освобождён,
        // _SSLClientContext = nullptr; деструктор a: refcount -> -1

    // исправленный код: контекст b жив, указатель тот же
    // баг: getClientContext() создаст НОВЫЙ контекст — указатель другой
    SSL_CTX *ctxAfter = a.clientContext();
    EXPECT_EQ(ctxAfter, ctxBefore)
        << "Контекст был освобождён, пока жив второй сокет (двойной декремент refcount)";
}

// BUG-2: гонка на статических контексте и счётчиках.
// Детектор — ThreadSanitizer: без синхронизации он обязан показать
// data race на _clientContextRefCount / _SSLClientContext.
// Тест сам по себе не ассертит счётчики (они private) — ассерты TSan
// в его логе и есть результат. Запускать сборкой build-tsan.
TEST(DISABLED_SslRefcount, ConcurrentCreateDestroy)
{
    constexpr int kWorkers = 4;
    constexpr int kIterations = 100;

    std::atomic<int> created{0};
    std::vector<std::thread> workers;

    for (int w = 0; w < kWorkers; ++w)
    {
        workers.emplace_back([&]
        {
            for (int i = 0; i < kIterations; ++i)
            {
                TestableSslSocket s(nullptr, "127.0.0.1", 443);
                if (s.instance())
                    ++created;
            }
        });
    }

    for (auto &t : workers)
        t.join();

    EXPECT_EQ(created.load(), kWorkers * kIterations);
}

// BUG-1: статический флаг _useExtrenalCerts затирается каждым конструктором.
// Наблюдаемый эффект появится при реактивации загрузки сертификатов в
// getServerContext(). Тест фиксирует сценарий конфликта, чтобы после
// исправления (флаг — поле экземпляра) он стал проверкой корректности.
TEST(DISABLED_SslCerts, StaticFlagOverwrittenByLaterInstance)
{
    TestableSslSocket external(nullptr, "127.0.0.1", 443, INVALID_DESCRIPTOR,
                               false, std::string(), std::string(),
                               /*useExternalCertificates=*/true);
    TestableSslSocket internal(nullptr, "127.0.0.1", 443, INVALID_DESCRIPTOR,
                               false, std::string(), std::string(),
                               /*useExternalCertificates=*/false);

    // здесь будет ассерт на конфигурацию каждого сокета после рефакторинга
    SUCCEED();
}

/**************************************************************************************************/


// ---------- Конструктор и состояние ----------

TEST(SslSocket, ClientSocketWithInvalidDescriptorIsDisconnected)
{
    TestableSslSocket socket(nullptr, kTestHost, kTestPort);

    EXPECT_EQ(socket.socketType(), TcpSocketType::Client);
    EXPECT_EQ(socket.connectionState(), HandlerBase::ConnectionState::Disconnected);
}

TEST(SslSocket, ServerSocketWithValidDescriptorIsConnecting)
{
    Descriptor d = makeServerDescriptor();
    ASSERT_NE(d, INVALID_DESCRIPTOR);

    TestableSslSocket socket(nullptr, kTestHost, kTestPort, d);

    EXPECT_EQ(socket.socketType(), TcpSocketType::Server);
    EXPECT_EQ(socket.connectionState(), HandlerBase::ConnectionState::Connecting);

    socket.close(); // закрывает реальный дескриптор
}

// ---------- Контексты: контракт singleton ----------

TEST(SslSocket, ClientInstancesShareContext)
{
    TestableSslSocket a(nullptr, kTestHost, kTestPort);
    TestableSslSocket b(nullptr, kTestHost, kTestPort);

    auto *instanceA = a.instance();
    auto *instanceB = b.instance();
    ASSERT_NE(instanceA, nullptr);
    ASSERT_NE(instanceB, nullptr);

    // SSL-объекты уникальны, контекст общий
    EXPECT_NE(instanceA, instanceB);
    EXPECT_EQ(SSL_get_SSL_CTX(instanceA), SSL_get_SSL_CTX(instanceB));
}

TEST(SslSocket, ServerInstancesShareContext)
{
    Descriptor d1 = makeServerDescriptor();
    Descriptor d2 = makeServerDescriptor();
    ASSERT_NE(d1, INVALID_DESCRIPTOR);
    ASSERT_NE(d2, INVALID_DESCRIPTOR);

    TestableSslSocket a(nullptr, kTestHost, kTestPort, d1);
    TestableSslSocket b(nullptr, kTestHost, kTestPort, d2);

    auto *instanceA = a.instance();
    auto *instanceB = b.instance();
    ASSERT_NE(instanceA, nullptr);
    ASSERT_NE(instanceB, nullptr);

    EXPECT_NE(instanceA, instanceB);
    EXPECT_EQ(SSL_get_SSL_CTX(instanceA), SSL_get_SSL_CTX(instanceB));

    a.close();
    b.close();
}

TEST(SslSocket, ClientAndServerContextsAreSeparate)
{
    Descriptor d = makeServerDescriptor();
    ASSERT_NE(d, INVALID_DESCRIPTOR);

    TestableSslSocket client(nullptr, kTestHost, kTestPort);
    TestableSslSocket server(nullptr, kTestHost, kTestPort, d);

    ASSERT_NE(client.instance(), nullptr);
    ASSERT_NE(server.instance(), nullptr);

    EXPECT_NE(SSL_get_SSL_CTX(client.instance()),
              SSL_get_SSL_CTX(server.instance()));
}

// ---------- Чтение и запись до соединения ----------

TEST(SslSocket, ReadWriteBeforeConnectionReturnZero)
{
    TestableSslSocket socket(nullptr, kTestHost, kTestPort);

    char buffer[64];

    // не подключено — SSL_read/SSL_write не вызываются, возвращается 0
    EXPECT_EQ(socket.read(buffer, sizeof(buffer)), 0);
    EXPECT_EQ(socket.write(buffer, sizeof(buffer)), 0);
}

// ---------- Информация о шифре до хендшейка ----------

// Контракт OpenSSL: SSL_CIPHER_get_version/name(NULL) возвращают "(NONE)".
// До хендшейка SSL_get_current_cipher возвращает nullptr,
// поэтому ожидаются именно заглушки.
TEST(SslSocket, CipherInfoBeforeHandshakeIsNone)
{
    TestableSslSocket socket(nullptr, kTestHost, kTestPort);
    ASSERT_NE(socket.instance(), nullptr);

    EXPECT_EQ(socket.getCipherVersion(), "(NONE)");
    EXPECT_EQ(socket.getCipherName(), "(NONE)");
}

// ---------- Жизненный цикл контекста ----------

// Корректное поведение без close(): у двух клиентов refcount == 2,
// уничтожение первого не освобождает контекст второго.
// Активный тест — обязан проходить уже сейчас.
TEST(SslSocket, ContextSurvivesWhenOneOfTwoClientsDestroyed)
{
    TestableSslSocket a(nullptr, kTestHost, kTestPort);
    SSL_CTX *ctxBefore = nullptr;
    {
        TestableSslSocket b(nullptr, kTestHost, kTestPort);
        ASSERT_NE(b.instance(), nullptr);
        ctxBefore = SSL_get_SSL_CTX(b.instance());
        ASSERT_NE(ctxBefore, nullptr);
    } // деструктор b: refcount 2 -> 1, контекст жив

    // контекст a должен остаться тем же
    ASSERT_NE(a.instance(), nullptr);
    EXPECT_EQ(SSL_get_SSL_CTX(a.instance()), ctxBefore);
}


