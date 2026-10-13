#include "SocketUtils.h"

#ifdef linux
#include <ifaddrs.h>
#endif

#include <string.h>

namespace Threader {

namespace Utils {


bool SocketUtils::_isSocketsInitialized = false;


InterfaceInfo::InterfaceInfo(std::string interfaceName_,
                             sa_family_t addressFamily_,
                             std::string ipAddress_)
    : interfaceName(interfaceName_)
    , addressFamily(addressFamily_)
    , ipAddress(ipAddress_)
{
}

bool SocketUtils::initializeSockets()
{
#ifdef WIN32
    if (_isSocketsInitialized)
        return true;

    uint16_t version = WINSOCK_VERSION;
    WSADATA wsaData;
    auto result = WSAStartup(version, &wsaData);
    _isSocketsInitialized = (0 == result);
#endif

#ifdef linux
    _isSocketsInitialized = true;
#endif
    return _isSocketsInitialized;
}

void SocketUtils::finalizeSockets()
{
#ifdef WIN32
   if (_isSocketsInitialized)
       WSACleanup();
#endif
}

bool SocketUtils::checkSocketsInitialization()
{
    if (_isSocketsInitialized)
        return true;
    return initializeSockets();
}

bool SocketUtils::isIpAddressString(const std::string &ipAddressString)
{
    if (!checkSocketsInitialization())
        return false;

    auto addr = inet_addr(ipAddressString.c_str());
    return (addr != INADDR_NONE && addr != INADDR_ANY);
}

std::string SocketUtils::sockAddrToString(const sockaddr_in &addr)
{
    return std::string(inet_ntoa(addr.sin_addr));
}

sockaddr_in SocketUtils::ipAddressStringToAddress(const std::string &ipAddress,
                                                  const uint16_t &port)
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    addr.sin_port = htons(port);
    return addr;
}

std::string SocketUtils::hostToIpAddressString(const std::string &host)
{
    std::string result = host;
    if (!checkSocketsInitialization())
        return result;

#ifdef WIN32

    auto entry = gethostbyname(host.c_str());
    if (!entry)
        return result;

    if (entry->h_addrtype == AF_INET)
    {
        int i = 0;
        struct in_addr addr;
        while (entry->h_addr_list[i] != 0) {
            addr.s_addr = *(u_long *) entry->h_addr_list[i++];
            result = std::string(inet_ntoa(addr));
            break;
        }
    }

#endif

#ifdef linux
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    int errcode = getaddrinfo(host.c_str(), nullptr, &hints, &res);
    if (errcode != 0)
        return result;

    auto ptr = (reinterpret_cast<struct sockaddr_in*>(res->ai_addr))->sin_addr;
    result = std::string(inet_ntoa(ptr));
    freeaddrinfo(res);

#endif
    return result;
}

#ifdef linux

InterfaceInfoList SocketUtils::interfacesInfo(const std::string &adapterName, const ProtocolFamilySet protocolFamilySet)
{
    struct ifaddrs* interfaceAdressesList = nullptr;
    InterfaceInfoList result;

    auto r = getifaddrs(&interfaceAdressesList);
    if (r != 0)
        return result;

    for(struct ifaddrs* interfaceAddress = interfaceAdressesList;
        interfaceAddress != nullptr;
        interfaceAddress = interfaceAddress->ifa_next)
    {
        std::string ipaddress_human_readable_form;
        std::string interfaceName = std::string(interfaceAddress->ifa_name);

        if (interfaceName == adapterName || adapterName.empty())
        {
            if (!interfaceAddress->ifa_addr)
                continue;

            sa_family_t addressFamily = interfaceAddress->ifa_addr->sa_family;

            bool processEntry = protocolFamilySet.empty()
                    || protocolFamilySet.count(addressFamily) > 0;
            if (!processEntry)
                continue;

            switch (addressFamily) {
            case AF_INET:
            {
                char buffer[INET_ADDRSTRLEN] = {0, };
                inet_ntop(addressFamily,
                          &((struct sockaddr_in*)(interfaceAddress->ifa_addr))->sin_addr,
                          buffer,
                          INET_ADDRSTRLEN);
                result.push_back(InterfaceInfo(interfaceName, addressFamily, std::string(buffer)));
                break;
            }
            case AF_INET6:
            {
                char buffer[INET6_ADDRSTRLEN] = {0, };
                inet_ntop(addressFamily,
                          &((struct sockaddr_in6*)(interfaceAddress->ifa_addr))->sin6_addr,
                          buffer,
                          INET6_ADDRSTRLEN);

                result.push_back(InterfaceInfo(interfaceName, addressFamily, std::string(buffer)));
                break;
            }
            default:
                break;
            }
        }
    }

    freeifaddrs(interfaceAdressesList);

    return result;

}

InterfaceInfo SocketUtils::interfaceInfo(const std::string &adapterName, const uint8_t protocolFamily)
{
    auto interfaces = interfacesInfo(adapterName, {protocolFamily});
    if (interfaces.empty())
        return InterfaceInfo();
    return interfaces.front();
}

#endif

int SocketUtils::lastError()
{
#ifdef WIN32
    return WSAGetLastError();
#endif
#ifdef linux
    return errno;
#endif
    return 0;
}

void SocketUtils::closeSocket(Descriptor socket)
{
#ifdef WIN32
    shutdown((SOCKET)socket, SD_BOTH);
    closesocket((SOCKET)socket);
#endif

#ifdef linux
    shutdown(socket, SHUT_RDWR);
    close(socket);
#endif
}

#ifdef WIN32

Descriptor SocketUtils::createTcpServerSocket(uint16_t port,
                                              bool isBlocking,
                                              sockaddr_in &addr,
                                              int &errorCode)
{
    if (!checkSocketsInitialization())
        return (Descriptor)INVALID_SOCKET;

    errorCode = 0;

    // создание сокета
    Descriptor result = (Descriptor)WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
    if (INVALID_SOCKET == (SOCKET)result)
    {
        errorCode = lastError();
        closeSocket(result);
        return INVALID_DESCRIPTOR;
    }

    // установка режима блокировки сокета
    u_long arg = (isBlocking) ? 0 : 1;
    if (ioctlsocket((SOCKET)result, FIONBIO, &arg) != 0)
    {
        errorCode = lastError();
        closeSocket(result);
        return INVALID_DESCRIPTOR;
    }

    // привязка сокета к порту
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

    if (bind((SOCKET)result, (struct sockaddr*)&addr, sizeof(addr)) != 0)
    {
        errorCode = lastError();
        closeSocket(result);
        return INVALID_DESCRIPTOR;
    }

    // запуск прослушивания запросов на подключение
    if (listen((SOCKET)result, SOMAXCONN) != 0)
    {
        errorCode = lastError();
        closeSocket(result);
        return INVALID_DESCRIPTOR;
    }

    return (Descriptor)result;
}

Descriptor SocketUtils::openTcpClientSocket(const std::string &ipAddress,
                                            const uint16_t port,
                                            int &errorCode,
                                            const int timeout)
{
    if (!checkSocketsInitialization())
        return (Descriptor)INVALID_SOCKET;

    errorCode = 0;

    // создание сокета
    Descriptor result = (Descriptor)WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
    if (INVALID_SOCKET == (SOCKET)result)
    {
        errorCode = lastError();
        return INVALID_DESCRIPTOR;
    }

    // неблокирующий режим
    if (!setBlockingMode(result, false))
    {
        errorCode = lastError();
        closeSocket(result);
        return INVALID_DESCRIPTOR;
    }

    // привязка сокета к порту
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

    // подключение
    int operationResult = connect((SOCKET)result, (struct sockaddr*)&addr, sizeof(addr));
    if (SOCKET_ERROR == operationResult)
    {
        errorCode = lastError();
        if (errorCode != WSAEWOULDBLOCK)
        {
            closeSocket(result);
            return INVALID_DESCRIPTOR;
        }

        if (0 != timeout)
        {
            Sleep(timeout);
            fd_set setw, sete;

            FD_ZERO(&setw);
            FD_SET((SOCKET)result, &setw);

            FD_ZERO(&sete);
            FD_SET((SOCKET)result, &sete);

            select(0, nullptr, &setw, &sete, nullptr);
            if (!FD_ISSET((SOCKET)result, &setw))
            {
                errorCode = lastError();
                closeSocket(result);
                return INVALID_DESCRIPTOR;
            }
        }
    }

    return result;
}

int SocketUtils::inputBytesAvalable(const Descriptor descriptor)
{
    if (descriptor != INVALID_DESCRIPTOR)
    {
        u_long result;
        ioctlsocket((SOCKET)descriptor, FIONREAD, &result);
        return result;
    }
    else
      return 0;
}

#endif

#ifdef linux

Descriptor SocketUtils::createTcpServerSocket(uint16_t port,
                                              bool isBlocking,
                                              sockaddr_in &addr,
                                              int &errorCode)
{
    errorCode = 0;

    // создание сокета, принимающего подключения
    Descriptor result = socket(AF_INET, SOCK_STREAM, 0);
    if (result < 0)
    {
        errorCode = lastError();
        closeSocket(result);
        return INVALID_DESCRIPTOR;
    }

    // установка признака повторного использования сокета
    int flag = 1;
    int callResult = setsockopt(result, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    if (callResult < 0)
    {
        errorCode = lastError();
        closeSocket(result);
        return INVALID_DESCRIPTOR;
    }

    // установка неблокирующего режима
    // все получаемые входящие сокеты тоже будут неблокирующие (ПИЗДЕЖЖЖ!!!)
    if (!isBlocking)
    {
        // перевод сокета в неблокирующий режим
        callResult = fcntl(result, F_SETFL, O_NONBLOCK);
        if (callResult != 0)
        {
            errorCode = lastError();
            closeSocket(result);
            return INVALID_DESCRIPTOR;
        }
    }

    // привязка сокета к порту
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

    callResult = bind(result, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (callResult < 0)
    {
        errorCode = lastError();
        closeSocket(result);
        return INVALID_DESCRIPTOR;
    }

    // запуск прослушивания запросов на подключение
    callResult = listen(result, 64);
    if (callResult < 0)
    {
        errorCode = lastError();
        closeSocket(result);
        return INVALID_DESCRIPTOR;
    }

    return result;
}

Descriptor SocketUtils::openTcpClientSocket(const std::string &ipAddress,
                                            const uint16_t port,
                                            int &errorCode,
                                            const int)
{
    struct addrinfo hints, *addr_result;
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICHOST;

    // получение информации об удаленном хосте
    int callResult = getaddrinfo(ipAddress.c_str(),
                                 std::to_string(port).c_str(),
                                 &hints,
                                 &addr_result);
    if (callResult != 0)
    {
        errorCode = lastError();
        return INVALID_DESCRIPTOR;
    }

    // создание сокета
    Descriptor result = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (result < 0)
    {
        errorCode = lastError();
        freeaddrinfo(addr_result);
        return INVALID_DESCRIPTOR;
    }

    // перевод сокета в неблокирующий режим
    callResult = fcntl(result, F_SETFL, O_NONBLOCK);
    if (callResult != 0)
    {
        errorCode = lastError();
        closeSocket(result);
        freeaddrinfo(addr_result);
        return INVALID_DESCRIPTOR;
    }

    // установка таймаутов на время ожидания при передаче сообщения об ошибке
    timespec t;
    t.tv_sec = 5;
    t.tv_nsec = 0;
    callResult = setsockopt(result, SOL_SOCKET, SO_SNDTIMEO, &t, sizeof(t));
    if (callResult != 0)
    {
        errorCode = lastError();
        closeSocket(result);
        freeaddrinfo(addr_result);
        return INVALID_DESCRIPTOR;
    }

    // установка таймаутов на время ожидания при приеме сообщения об ошибке
    callResult = setsockopt(result, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));
    if (callResult != 0)
    {
        errorCode = lastError();
        closeSocket(result);
        freeaddrinfo(addr_result);
        return INVALID_DESCRIPTOR;
    }

    // установка количества повторных передач SYN, которое должен послать TCP
    // до отмены попытки установки соединения
    int32_t synCount = 3;
    callResult = setsockopt(result, IPPROTO_TCP, TCP_SYNCNT, &synCount, sizeof(synCount));
    if (callResult != 0)
    {
        errorCode = lastError();
        closeSocket(result);
        freeaddrinfo(addr_result);
        return INVALID_DESCRIPTOR;
    }

    callResult = connect(result, addr_result->ai_addr, addr_result->ai_addrlen);
    if (callResult == -1)
    {
        errorCode = lastError();
        if (errorCode != EINPROGRESS)
        {
            closeSocket(result);
            result = INVALID_DESCRIPTOR;
        }
        freeaddrinfo(addr_result);
        return result;
    }

    return result;
}

#endif

Descriptor SocketUtils::createUdpSocket(sockaddr_in &bindAddress,
                                        int &errorCode)
{
    errorCode= 0;
#ifdef WIN32
    if (!checkSocketsInitialization())
        return (Descriptor)INVALID_SOCKET;
    Descriptor result = (Descriptor)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#endif
#ifdef linux
    Descriptor result = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#endif

    // NOTE: Проверить возвращаемые значения для Windows
    if (INVALID_DESCRIPTOR == result)
    {
        errorCode = lastError();
        return result;
    }

#ifdef WIN32
    auto callResult = bind((SOCKET)result,
                           reinterpret_cast<sockaddr*>(&bindAddress),
                           sizeof(bindAddress));
#endif
#ifdef linux
    auto callResult = bind(result,
                           reinterpret_cast<sockaddr*>(&bindAddress),
                           sizeof(bindAddress));
#endif
    if (callResult < 0)
    {
        errorCode = lastError();
        closeSocket(result);
        return INVALID_DESCRIPTOR;
    }

    return result;
}

#ifdef linux

int SocketUtils::inputBytesAvalable(const Descriptor descriptor)
{
    if (descriptor != INVALID_DESCRIPTOR)
    {
        int32_t result;
        auto callResult = ioctl(descriptor, FIONREAD, &result);
        return  (0 == callResult) ? result : 0;
    }
    else
      return 0;
}

#endif

bool SocketUtils::setBlockingMode(Descriptor socket, bool isBlocking)
{

#ifdef WIN32
    u_long arg = (isBlocking) ? 0 : 1;
    return ioctlsocket((SOCKET)socket, FIONBIO, &arg) == 0;
#endif

#ifdef linux
    int arg = fcntl(socket, F_GETFL, 0);
    arg =(isBlocking)
            ? arg & ~O_NONBLOCK
            : arg | O_NONBLOCK;
    auto result = fcntl(socket, F_SETFL, arg);
    return 0 == result;
#endif

    return false;
}

}}
