#include "HandlerSslSocket.h"

#include "FileUtils.h"
#include "SocketUtils.h"

#include <cstring>

#include <openssl/x509.h>

#define PROCESS_ACCEPT_COUT 10

namespace Threader::Threads {

using namespace Threader::Utils;

constexpr char CERRTIFICATE[] =  R"STRING(-----BEGIN CERTIFICATE-----
MIID+TCCAuGgAwIBAgIUdrtvkf7vWUIZZcdozcXnwBNRvjwwDQYJKoZIhvcNAQEL
BQAwgYsxCzAJBgNVBAYTAlJVMQwwCgYDVQQIDANTUEIxFzAVBgNVBAcMDlN0Li1Q
ZXRlcnNidXJnMRwwGgYDVQQKDBNTWVNURU0gVEVDSE5PTE9HSUVTMRMwEQYDVQQD
DApzeXN0ZW10LnJ1MSIwIAYJKoZIhvcNAQkBFhNtYXJ0eW5vdkBzeXN0ZW10LnJ1
MB4XDTIyMTEyMzExMzMxMVoXDTMyMTEyMDExMzMxMVowgYsxCzAJBgNVBAYTAlJV
MQwwCgYDVQQIDANTUEIxFzAVBgNVBAcMDlN0Li1QZXRlcnNidXJnMRwwGgYDVQQK
DBNTWVNURU0gVEVDSE5PTE9HSUVTMRMwEQYDVQQDDApzeXN0ZW10LnJ1MSIwIAYJ
KoZIhvcNAQkBFhNtYXJ0eW5vdkBzeXN0ZW10LnJ1MIIBIjANBgkqhkiG9w0BAQEF
AAOCAQ8AMIIBCgKCAQEAq+mU8cCpO1/o5fThVplD5rmWXTXTu7H34YXy8gF50BPU
tR29eAbfdYS9JVBK9yy1QOUst6zX+ANx4FByAt0/KbcdCQE82hnl7PnRAiBNmYYD
AizBNGplG3+kqP2TjNUWY20PlB9zlWLpYndAb9LZCOVdKD4Zv+CAnefQwkWOzoDJ
eVNgPHX+lBfLKv375FvdHrQk2wCtV6uYrt7SiXOqGpxfvj9IZgjPP5GHN6xfZS9l
wWzV55myXR1eyHwRSYSYq9HNwESoIWIvk97bxs7Zq1XQtx2zochxTT+LQ++1BAoY
5ZIdgEEeA7oNBgCGJcRYJK3q4g7mRhfZcMGaNoFDWwIDAQABo1MwUTAdBgNVHQ4E
FgQU15eg6ltxyju3Jh2ONGKb3YiefvMwHwYDVR0jBBgwFoAU15eg6ltxyju3Jh2O
NGKb3YiefvMwDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAMLEe
NqAxeWKcvojH9zeBCkiVzfOHDQJIb7crqcDzQjuXHoz1wRb448wQLMs91i5yHaCH
Ft/oxd7KI7MesDGoqccNhjAlMJjX/4E1jY0ZFhXdzZYpTXKn35DWynsedI1kkF9y
5uVik0GkAYRMrqSpk1iL85EDyFwBTPMfrs0kOgcwyd9jBW+iAFN2w8Vq3GUZLiF7
DwCG3an/Ra742mr6kE+dJ/3pNbpNDxHakCkrh1SL96J3b1J9pyA0ecO/FLFlixZg
syC4noBqPoL48flbkHe0Z1ILRAKbPRWz1aG2jIsw3PD6Knp189yb63UrOIlJ0zKH
yA/FPotFyvhX9W3XsA==
-----END CERTIFICATE-----
)STRING";

constexpr char PRIVATE_KEY[] =  R"STRING(-----BEGIN PRIVATE KEY-----
MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCr6ZTxwKk7X+jl
9OFWmUPmuZZdNdO7sffhhfLyAXnQE9S1Hb14Bt91hL0lUEr3LLVA5Sy3rNf4A3Hg
UHIC3T8ptx0JATzaGeXs+dECIE2ZhgMCLME0amUbf6So/ZOM1RZjbQ+UH3OVYuli
d0Bv0tkI5V0oPhm/4ICd59DCRY7OgMl5U2A8df6UF8sq/fvkW90etCTbAK1Xq5iu
3tKJc6oanF++P0hmCM8/kYc3rF9lL2XBbNXnmbJdHV7IfBFJhJir0c3ARKghYi+T
3tvGztmrVdC3HbOhyHFNP4tD77UEChjlkh2AQR4Dug0GAIYlxFgkreriDuZGF9lw
wZo2gUNbAgMBAAECggEABob45nDykVQ5QI9FWwdjYkhGTZsJ7gTPgkWWuC+W03AH
/H8jlCzBS/H9SIRoRELzTYJ7addAiqR+12Mr4QX2LFGijZakG9fYjgsyL2Al8h/6
JJlUa0wQM19rqMBFW5c09r4hAEvCv2JLtYxiQEdY1uihL9z5F1BL3IhSZH3wR9Dg
qh3sCsSxlprAYSEJ2GB+Uct/bEjRszLWri++dIH600CTYaO0RjZlr27Zff1uZhTp
jeuIoENp/IU9oyKHCXhjrQrJ9SBYeJjF/as0C4JGp4BpPJVFvYvZrVgBCNyYhjVy
M4SI25QSsbB8wtrC5yxMVY0n77sPuDGeN4w+oCA6gQKBgQC+wqxlPjasbhJW/pli
jEU8QNb8oxTZJX8CENE6ViyAvMDdvPot+8JDxHRyrIraGfT3/uK16by5oaI6Y6eW
WkmkxGAXd9idQNsyFhDYdNaiKdEkBmSbNy7jvxna3/tyqB0uRuy14yWjp2NAAklN
8fcc+XSIQjlOn/59VTOrZuJXGwKBgQDmtL115e0eBrX2oz695HNaxpAuLHE7UHfo
tpBBPZ/Gk+5fYX0Cn8XRbCzc5t+jLeAaaPTKLR2FNGkNpyu0cdgargYH6OsJt45N
hV0UfxnXgeOFzV/L5Grs3vDfYtxIYWpajpb2gKalj3xxYbqJYapfBVkgAQWgF9DA
tRKYl/pIwQKBgBZgr+iQi8legDhEVaoB2n1e/dlVKDTw+R086850HXRcsCNS7+ZF
Clz4xtBM/Vzofpw5Ak9kxAbity0yU46HuOvjPYKDRJ3t5cjSXdeGnUcYsLgRJ+wt
ZQrCeD382HpyzjmHiG0t4FR6ZmIz4nWKVPrJKKpzWlWm3yvEuvL4dsTRAoGARv06
hzq22dRqzXex6LCRJk+PMgYO2ZeLLhpvDQQvU+VVJhhTZSlQr34/18Fa81U8biP5
nvR+7uwPn1nfQuj8/EisJmbvW9AVg5kT6SirBPs8zYFChvTEShrXt4UuDz+agD78
COHRZYRfYGwYAAoDaLW2lPh7iKWLYKDXFgk9xIECgYEAuMQMebPTFDQiXjOOAcEF
DkHMCLbpVlNMOelPoEc52KU05Y5oO3wOzcikQiUlpaZiX6pYxmyfNwgE+4mV1yQd
VtbmqNUkp+nIXnEM4FCOt7eYTDsYKCTzfO8D8kI0tV24s/XD9mOlVsaxw9xK+KzJ
LuV07YZafuosoKnufUwean0=
-----END PRIVATE KEY-----
)STRING";

HandlerSslSocket::HandlerSslSocket(HandlerDataConsumer *consumer,
                                   const std::string &host,
                                   const uint16_t &port,
                                   const Descriptor &descriptor,
                                   const bool verifyCertificate,
                                   const std::string &certificateFilename,
                                   const std::string &primaryKeyFilename,
                                   const bool useExternalCertificates)
    : HandlerTcpSocket(consumer, host, port, descriptor)

    , _sslContext(nullptr)
    , _sslInstance(nullptr)

    , _verifyCertificate(verifyCertificate)
    , _useExtrenalCertificates(useExternalCertificates)
    , _certificateFilename(certificateFilename)
    , _privateKeyFilename(primaryKeyFilename)
#ifdef WIN32
    , _acceptInProcess(false)
#endif
    , _acceptCounter(0)
{
    sslInitialize();

    if (INVALID_DESCRIPTOR == this->descriptor())
        setConnectionState(ConnectionState::Disconnected);
    else
        setConnectionState(ConnectionState::Connecting);
}

HandlerSslSocket::~HandlerSslSocket()
{
    if (ConnectionState::Disconnected != connectionState())
        close();

    sslFinalize();
}

#ifdef WIN32

bool HandlerSslSocket::open()
{
    if (TcpSocketType::Client != socketType())
        return false;

    setConnectionState(ConnectionState::Connecting);

    int errorCode(0);
    Descriptor _descriptor = openTcpClientSocket(ipAddress(), port(), errorCode);

    if (0 != errorCode && WSAEWOULDBLOCK != errorCode)
    {
        doError(errorCode);
        setConnectionState(ConnectionState::Disconnected);
    }
    else
    {
        setDescriptor(_descriptor);
        registerEvent();
    }

    return _descriptor != INVALID_DESCRIPTOR;
}

DescriptorsVector *HandlerSslSocket::events()
{
    auto eventsVector = HandlerBase::events();

    if (descriptor() == INVALID_DESCRIPTOR || eventsVector->size() < 1)
        return eventsVector;

    // событие закрытия сокета принимается всегда
    long mask = FD_CLOSE;

    // если сокет подключен и
    if (isConnected())
    {
        // включение ожидания получения данных
        mask |= FD_READ;
        // если есть данные на отправку
        if (needsToWrite())
        {
            mask |= FD_WRITE;
            // вызов события готовности к отправке данных
            doReadyWrite();
        }
    }
    else
    {
        // если клиентский сокет не подключен, то включение ожидания подключения
        if (TcpSocketType::Client == socketType())
            mask |= FD_CONNECT;

        if ((TcpSocketType::Server == socketType()) &&
            (ConnectionState::Connecting == connectionState()))
        {
            if (_acceptInProcess)
                processAccept();
            else
            {
                if (SSLInstance())
                {
                    if (setInstanceDescriptor())
                        processAccept();
                    else
                        doHandlerError();
                }
                else
                    doHandlerError();
            }
        }
    }

    // установка маски на событие сокета
    if (descriptor() != INVALID_DESCRIPTOR && eventsVector->size() > 0)
    {
        int operationResult = WSAEventSelect((SOCKET)descriptor(), eventsVector->at(0), mask);
        if (operationResult != 0)
            doError(SocketUtils::lastError());
    }
    else
    {
        doError(SocketUtils::lastError());
    }

    return eventsVector;
}

void HandlerSslSocket::doHandlerError(const int error)
{
    int sslError;
    if (error >= 0)
    {
        switch(error)
        {
        case SSL_ERROR_NONE:
            sslError = ERROR_SUCCESS;
            break;
        case SSL_ERROR_WANT_READ:
        case SSL_ERROR_WANT_WRITE:
            sslError = ERROR_MORE_DATA;
            break;
        case SSL_ERROR_SYSCALL:
            sslError = GetLastError();
            break;
        case SSL_ERROR_ZERO_RETURN:
            sslError = ERROR_NO_DATA;
            break;
        case ERROR_TIMEOUT:
            sslError = error;
            break;
        case SSL_ERROR_SSL:
        default:
            sslError = ERROR_GEN_FAILURE;
            break;
        }
    }
    else
        sslError = SocketUtils::lastError();
    // TODO: Close client socket and return error here?
    doError(sslError);
}

Descriptor HandlerSslSocket::openTcpClientSocket(const std::string &ipAddress,
                                                 const uint16_t port,
                                                 int &errorCode,
                                                 const int &timeout)
{
    if (!SocketUtils::checkSocketsInitialization())
        return (Descriptor)INVALID_SOCKET;

    errorCode = 0;

    // создание сокета
    auto result = Descriptor(WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
                                       nullptr, 0, WSA_FLAG_OVERLAPPED));
    if (INVALID_SOCKET == SOCKET(result))
        return INVALID_DESCRIPTOR;

    // блокирующий режим
    if (!SocketUtils::setBlockingMode(result, true))
    {
        doHandlerError();
        return INVALID_DESCRIPTOR;
    }

    //     установка таймаутов на время ожидания при передаче сообщения об ошибке
    DWORD to(5000); // 5 sec
    auto callResult = setsockopt(SOCKET(result), SOL_SOCKET, SO_SNDTIMEO,
                                 reinterpret_cast<const char *>(&to), sizeof(to));
    if (callResult != 0)
    {
        doHandlerError();
        return INVALID_DESCRIPTOR;
    }

    // установка таймаутов на время ожидания при приеме сообщения об ошибке
    callResult = setsockopt(SOCKET(result), SOL_SOCKET, SO_RCVTIMEO,
                            reinterpret_cast<const char *>(&to), sizeof(to));
    if (callResult != 0)
    {
        doHandlerError();
        return INVALID_DESCRIPTOR;
    }

    // привязка сокета к порту
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

    // подключение
    auto operationResult = ::connect(SOCKET(result), (struct sockaddr*)&addr,
                                     sizeof(addr));
    if (SOCKET_ERROR == operationResult)
    {
        errorCode = SocketUtils::lastError();
        if (errorCode != WSAEWOULDBLOCK)
        {
            close();
            return INVALID_DESCRIPTOR;
        }

        if (0 != timeout)
        {
            Sleep(timeout);
            fd_set setw, sete;

            FD_ZERO(&setw);
            FD_SET(SOCKET(result), &setw);

            FD_ZERO(&sete);
            FD_SET(SOCKET(result), &sete);

            select(0, nullptr, &setw, &sete, nullptr);
            if (!FD_ISSET(SOCKET(result), &setw))
            {
                errorCode = SocketUtils::lastError();
                close();
                return INVALID_DESCRIPTOR;
            }
        }
    }

    if (!SSLInstance())
    {
        doHandlerError();
        return INVALID_DESCRIPTOR;
    }

    if (1 != SSL_set_fd(SSLInstance(), int(SOCKET(result))))
    {
        doHandlerError();
        return INVALID_DESCRIPTOR;
    }

    if (_verifyCertificate)
    {
        if (!FileUtils::isRegularFileExists(_certificateFilename))
        {
            doError(1, "CA Certification file not found");
            return INVALID_DESCRIPTOR;
        }

        SSL_CTX_load_verify_locations(_sslContext, _certificateFilename.c_str(), nullptr);
        SSL_CTX_set_verify(_sslContext, SSL_VERIFY_PEER, nullptr);
    }

    auto lRet = connect();
    if (1 != lRet)
    {
        auto sslError = SSL_get_error(SSLInstance(), lRet);
        if (sslError != SSL_ERROR_WANT_READ)
        {
            doHandlerError(sslError);
            return INVALID_DESCRIPTOR;
        }
    }
    else
    {
        if (!SocketUtils::setBlockingMode(result, false))
        {
            doHandlerError();
            return INVALID_DESCRIPTOR;
        }

        if (_verifyCertificate)
        {
            auto verResult = SSL_get_verify_result(SSLInstance());
            std::string verStr = X509_verify_cert_error_string(verResult);
            if (verResult != X509_V_OK)
            {
                doError(int(verResult), verStr);
                return INVALID_DESCRIPTOR;
            }
        }

        // установка состояния подключения - подключено
        // событие изменения состояния вызовется автоматически
        setConnectionState(ConnectionState::Connected);
    }

    return result;
}

void HandlerSslSocket::processAccept()
{
    int ret_code(0);

    if (1 == (ret_code = accept()))
    {
        _acceptInProcess = false;
        setConnectionState(ConnectionState::Connected);
    }
    else
    {
        auto sslError = SSL_get_error(SSLInstance(), ret_code);
        if (SSL_ERROR_WANT_READ == sslError || SSL_ERROR_WANT_WRITE == sslError)
        {
            if (++_acceptCounter > PROCESS_ACCEPT_COUT)
            {
                doHandlerError(ERROR_TIMEOUT);
                _acceptInProcess = false;
            }
            else
                _acceptInProcess = true;
        }
        else
            doHandlerError(sslError);
    }
}

#endif

#ifdef linux

bool HandlerSslSocket::open()
{
    if (TcpSocketType::Client != socketType())
        return false;

    setConnectionState(ConnectionState::Connecting);

    int errorCode(0);
    auto descriptor = openTcpClientSocket(ipAddress(), port(), errorCode);
    if (0 != errorCode && EINPROGRESS != errorCode)
    {
        setConnectionState(ConnectionState::Disconnected);
        doError(errorCode);
    }
    else
        setDescriptor(descriptor);

    return  descriptor != INVALID_DESCRIPTOR;
}

bool HandlerSslSocket::process(const pollfd &event)
{
    auto d = descriptor();

    if (ConnectionState::Disconnected == connectionState() || INVALID_DESCRIPTOR == d)
        return false;

    if (event.fd != d)
        return false;

    // если произошла ошибка, то её нужно отработать в первую очередь
    // пока всё ещё живо
    if (event.revents & (POLLERR | POLLNVAL))
    {
        doError(SocketUtils::lastError());
        return false;
    }

    // шаг TLS-хендшейка сервера: пришли данные от клиента (ClientHello
    // или очередные раунды обмена ключами)
    if (connectionState() == ConnectionState::Connecting &&
        socketType() == TcpSocketType::Server &&
        (event.revents & POLLIN))
    {
        int ret = accept();
        if (ret == 1)
        {
            setConnectionState(ConnectionState::Connected);
            return true;
        }

        auto sslError = SSL_get_error(_sslInstance, ret);
        if (sslError == SSL_ERROR_WANT_READ || sslError == SSL_ERROR_WANT_WRITE)
        {
            // хендшейк не завершён: клиент ещё шлёт данные, ждём следующий
            // POLLIN. Лимит попыток — защита от зацикливания/атаки
            // «вечно недокрученный хендшейк» (референс: processAccept)
            if (++_acceptCounter > PROCESS_ACCEPT_COUT)
            {
                doError(ETIMEDOUT);
                return false;
            }
            return true;
        }

        doError(sslError);  // реальная ошибка TLS — рвём соединение
        return false;
    }

    // следующим нужно проверять закрытие удаленной стороной
    if (event.revents & (POLLHUP | POLLRDHUP))
    {
        // прочитать всё, что есть на входе
        doReadyRead();
        // закрыть подключение
        close();
        // с вещами на выход
        return true;
    }

    // если есть возможность записать или произошло подключение
    if (event.revents & POLLOUT)
    {
        // TODO: Как может прийти POLLOUT при не ConnectionState::Connecting?
        // обработка события подключения
        if (connectionState() == ConnectionState::Connecting)
        {
            // чтение готовности сокета, проверка ошибки отложенного открытия
            int32_t error = 0;
            uint32_t errorLength = sizeof(error);
            if (getsockopt(d, SOL_SOCKET, SO_ERROR, &error, &errorLength) == 0)
            {
                // проверка готовности сокета
                if (error == 0)
                {
                    setConnectionState(ConnectionState::Connected);
                    uint32_t aliveTimeout = 10000;
                    setsockopt(d, SOL_TCP, TCP_USER_TIMEOUT, &aliveTimeout, sizeof(aliveTimeout));
                    if (socketType() == TcpSocketType::Client)
                    {
                        // TODO: пока оставить, Т!
                        if (!SocketUtils::setBlockingMode(d, true))
                        {
                            doError(SocketUtils::lastError());
                            return false;
                        }

                        if (!_sslInstance)
                        {
                            doError(SocketUtils::lastError());
                            return false;
                        }

                        if (!setInstanceDescriptor())
                        {
                            doError(SocketUtils::lastError());
                            return false;
                        }

                        if (_verifyCertificate)
                        {
                            if (!FileUtils::isRegularFileExists(_certificateFilename))
                            {
                                doError(1, "CA Certification file not found");
                                return false;
                            }
                            //                            SSL_CTX_load_verify_file(getClientContext(), _caCertificateFileName.c_str());
                            SSL_CTX_load_verify_locations(_sslContext,
                                                          _certificateFilename.c_str(),
                                                          nullptr);

                            SSL_CTX_set_verify(_sslContext, SSL_VERIFY_PEER, nullptr);
                        }

                        if (!connect())
                        {
                            doError(SocketUtils::lastError());
                            return false;
                        }

                        if (!SocketUtils::setBlockingMode(d, false))
                        {
                            doError(SocketUtils::lastError());
                            return false;
                        }

                        if (_verifyCertificate)
                        {
                            auto verificationResult = SSL_get_verify_result(_sslInstance);
                            std::string verStr(X509_verify_cert_error_string(verificationResult));

                            if (verificationResult != X509_V_OK)
                            {
                                doError(int(verificationResult), verStr);
                                return false;
                            }
                        }
                    }
                }
                else
                {
                    doError(SocketUtils::lastError());
                    return false;
                }
            }
            else
            {
                doError(SocketUtils::lastError());
                return false;
            }
        }

        // есть возможность отправить данные
        doReadyWrite();
    }

    // если есть что прочитать
    if (event.revents & POLLIN)
        doReadyRead();

    return true;
}

void HandlerSslSocket::assign(pollfd &event)
{
    auto connectionStateLocal = connectionState();
    if ((socketType() == TcpSocketType::Server) &&
        (connectionStateLocal == ConnectionState::Connecting))
    {
        // BUG-11: блокирующий SSL_accept здесь больше не выполняется.
        // Хендшейк становится асинхронным: шаг выполняет process()
        // по событию POLLIN, WANT_READ/WANT_WRITE — норма ожидания.
        if (!_sslInstance || !setInstanceDescriptor())
        {
            doError(SocketUtils::lastError());
            setConnectionState(ConnectionState::Disconnected);
        }
        else if (!SocketUtils::setBlockingMode(descriptor(), false))
        {
            doError(SocketUtils::lastError());
            setConnectionState(ConnectionState::Disconnected);
        }
        else
        {
            // первая попытка без ожидания: если ClientHello уже в буфере,
            // хендшейк завершится прямо здесь; иначе WANT_READ — норма
            int ret = accept();
            if (ret == 1)
                setConnectionState(ConnectionState::Connected);
            else if (auto sslError = SSL_get_error(_sslInstance, ret);
                     sslError != SSL_ERROR_WANT_READ &&
                     sslError != SSL_ERROR_WANT_WRITE)
            {
                doError(sslError);
                setConnectionState(ConnectionState::Disconnected);
            }
        }
    }

    if (connectionStateLocal == ConnectionState::Disconnected)
        return;

    event.fd = descriptor();
    event.revents = 0;
    event.events = POLLIN | POLLERR | POLLHUP | POLLNVAL | POLLRDHUP;

    // POLLOUT нужен только клиенту (ожидание завершения TCP-connect).
    // Серверу в Connecting POLLOUT не даём: подключённый сокет всегда
    // готов к записи — poll зациклится на 100% CPU.
    if ((connectionStateLocal == ConnectionState::Connecting &&
         socketType() == TcpSocketType::Client) || needsToWrite())
        event.events |= POLLOUT;
}

#endif

void HandlerSslSocket::close()
{
    HandlerTcpSocket::close();
}

int HandlerSslSocket::read(void *data, int count)
{
    if (ConnectionState::Connected == connectionState())
        return int(::SSL_read(_sslInstance, data, count));

    return 0;
}

int HandlerSslSocket::write(const void *data, int count)
{
    if (ConnectionState::Connected == connectionState())
        return int(::SSL_write(_sslInstance, data, count));

    return 0;
}

std::string HandlerSslSocket::getCipherVersion()
{
    return SSL_CIPHER_get_version(SSL_get_current_cipher(_sslInstance));
}

std::string HandlerSslSocket::getCipherName()
{
    return SSL_get_cipher_name(_sslInstance);
}

bool HandlerSslSocket::setInstanceDescriptor()
{
#ifdef linux
    return 1 == SSL_set_fd(_sslInstance, descriptor()) ? true : false;
#endif
#ifdef WIN32
    return 1 == SSL_set_fd(_sslInstance, int(SOCKET(descriptor()))) ? true : false;
#endif
    return false;
}

int HandlerSslSocket::connect()
{
    return SSL_connect(_sslInstance);
}

int HandlerSslSocket::accept()
{
    return SSL_accept(_sslInstance);
}

#ifdef linux

Descriptor HandlerSslSocket::openTcpClientSocket(const std::string &ipAddress, const uint16_t port, int &errorCode)
{
    struct addrinfo hints, *addr_result;
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICHOST;

    // получение информации об удаленном хосте
    int callResult = getaddrinfo(ipAddress.c_str(),
                                 std::to_string(port).c_str(),
                                 //                                 QString::number(port).toLocal8Bit().constData(),
                                 &hints,
                                 &addr_result);
    if (callResult != 0)
    {
        errorCode = SocketUtils::lastError();
        return INVALID_DESCRIPTOR;
    }

    // создание сокета
    Descriptor result = socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK*/, 0);
    if (result < 0)
    {
        errorCode = SocketUtils::lastError();
        freeaddrinfo(addr_result);
        return INVALID_DESCRIPTOR;
    }

    // перевод сокета в неблокирующий режим
    // TODO: пока оставить, Т!
    // !!! Перенесено в функцию process (), так как при TLS/SSL не сработает при неблокирующем
    //    callResult = 0;
    //    callResult = fcntl (result, F_SETFL, O_NONBLOCK);
    //    if (callResult != 0)
    //    {
    //        errorCode = lastError ();
    //        closeSocket (result);
    //        freeaddrinfo (addr_result);
    //        return INVALID_DESCRIPTOR;
    //    }

    // установка таймаутов на время ожидания при передаче сообщения об ошибке
    timespec t;
    t.tv_sec = 5;
    t.tv_nsec = 0;
    callResult = setsockopt(result, SOL_SOCKET, SO_SNDTIMEO, &t, sizeof(t));
    if (callResult != 0)
    {
        errorCode = SocketUtils::lastError();
        SocketUtils::closeSocket(result);
        freeaddrinfo(addr_result);
        return INVALID_DESCRIPTOR;
    }

    // установка таймаутов на время ожидания при приеме сообщения об ошибке
    callResult = setsockopt(result, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));
    if (callResult != 0)
    {
        errorCode = SocketUtils::lastError();
        SocketUtils::closeSocket(result);
        freeaddrinfo(addr_result);
        return INVALID_DESCRIPTOR;
    }

    // установка количества повторных передач SYN, которое должен послать TCP
    // до отмены попытки установки соединения
    int32_t synCount = 3;
    callResult = setsockopt(result, IPPROTO_TCP, TCP_SYNCNT, &synCount, sizeof(synCount));
    if (callResult != 0)
    {
        errorCode = SocketUtils::lastError();
        SocketUtils::closeSocket(result);
        freeaddrinfo(addr_result);
        return INVALID_DESCRIPTOR;
    }

    callResult = ::connect(result, addr_result->ai_addr, addr_result->ai_addrlen);
    if (callResult == -1)
    {
        errorCode = SocketUtils::lastError();
        if (errorCode != EINPROGRESS)
        {
            SocketUtils::closeSocket(result);
            result = INVALID_DESCRIPTOR;
        }
        freeaddrinfo(addr_result);
        return result;
    }
    return result;
}

#endif

void HandlerSslSocket::sslInitialize()
{
    static bool initialized = false;
    if (!initialized)
    {
        static std::once_flag once;
        std::call_once(once, []() {
            OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS
                                 | OPENSSL_INIT_ADD_ALL_CIPHERS | OPENSSL_INIT_ADD_ALL_DIGESTS,
                             nullptr);
            OpenSSL_add_all_algorithms();
            SSL_load_error_strings();
            ERR_load_crypto_strings();
        });
        initialized = true;
    }

    switch (socketType())
    {
    case TcpSocketType::Server:
    {
        _sslContext = SSL_CTX_new(TLS_server_method());

        if (!_sslContext)
            break;

        // проверка существования файлов внешних сертификатов
        if (_useExtrenalCertificates)
        {
            _useExtrenalCertificates = FileUtils::isRegularFileExists(_certificateFilename)
                                       && FileUtils::isRegularFileExists(_privateKeyFilename);
        }


        // если использовать встроенные сертификаты
        if (!_useExtrenalCertificates)
        {
            // формирование файлов встроенных сертификатов
#ifdef linux
            char certificateFileName[] = "/tmp/certificateXXXXXX";
            mkstemp(certificateFileName);

            char privateKeyFilename[] = "/tmp/privatekeyXXXXXX";
            mkstemp(privateKeyFilename);
#endif

            _certificateFilename = certificateFileName;
            _privateKeyFilename = privateKeyFilename;

            auto isWritten = FileUtils::fileReplaceContent(_certificateFilename, CERRTIFICATE)
                             && FileUtils::fileReplaceContent(_privateKeyFilename, PRIVATE_KEY);

            if (!isWritten)
            {
                FileUtils::fileDelete(_certificateFilename);
                FileUtils::fileDelete(_privateKeyFilename);
                break;
            }
        }

        auto certificateResult = SSL_CTX_use_certificate_file(_sslContext, _certificateFilename.c_str(), SSL_FILETYPE_PEM);
        auto privateKeyResult = SSL_CTX_use_PrivateKey_file(_sslContext, _privateKeyFilename.c_str(), SSL_FILETYPE_PEM);

        if (!_useExtrenalCertificates)
        {
            FileUtils::fileDelete(_certificateFilename);
            FileUtils::fileDelete(_privateKeyFilename);
        }

        if (certificateResult != 1 || privateKeyResult != 1)
            break;

        break;
    }
    case TcpSocketType::Client:
    {
        _sslContext = SSL_CTX_new(TLS_client_method());
        break;
    }
    }

    _sslInstance = SSL_new(_sslContext);
}

void HandlerSslSocket::sslFinalize()
{
    if (_sslInstance)
    {
        SSL_shutdown(_sslInstance);
        SSL_free(_sslInstance);
        _sslInstance = nullptr;
    }

    if (_sslContext)
    {
        SSL_CTX_free(_sslContext);
        _sslContext = nullptr;
    }
}

SSL_CTX *HandlerSslSocket::sslContext() const
{
    return _sslContext;
}

bool HandlerSslSocket::useExtrenalCerts() const
{
    return _useExtrenalCertificates;
}


SSL *HandlerSslSocket::sslInstance()
{
    return _sslInstance;
}

}

