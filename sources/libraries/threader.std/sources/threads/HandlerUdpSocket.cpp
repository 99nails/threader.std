#include "HandlerUdpSocket.h"

#include "SocketUtils.h"

#ifdef WIN32
#include <ws2tcpip.h>
#endif

#include <limits>
#include <string.h>

namespace Threader {

namespace Threads {


using namespace Threader::Utils;

constexpr int MAXIMUM_DATA_SIZE = std::numeric_limits<uint16_t>::max();

HandlerUdpSocket::HandlerUdpSocket(HandlerDataConsumer *consumer,
                                   const std::string &host,
                                   const uint16_t port,
                                   const UdpSocketType udpSocketType,
                                   const std::string &bindIpAddress)

    : HandlerBase(consumer,
                  host + ":" + std::to_string(port),
                  INVALID_DESCRIPTOR,
                  true)
    , _host(host)
    , _port(port)
    , _socketType(udpSocketType)
{
    _ipAddress = _host;
    if (!SocketUtils::isIpAddressString(_host))
        _ipAddress = SocketUtils::hostToIpAddressString(_host);

    _bindAddress = SocketUtils::ipAddressStringToAddress(bindIpAddress, _port);
    _remoteAddress = SocketUtils::ipAddressStringToAddress(_ipAddress, _port);

    memset(&_readAddress, 0, sizeof(_readAddress));

    if (UdpSocketType::Sender == _socketType)
        _bindAddress.sin_port = 0;
}

std::string HandlerUdpSocket::host() const
{
    return _host;
}

uint16_t HandlerUdpSocket::port() const
{
    return _port;
}

std::string HandlerUdpSocket::ipAddress() const
{
    return _ipAddress;
}

UdpSocketType HandlerUdpSocket::socketType() const
{
    return _socketType;
}

bool HandlerUdpSocket::open()
{
    int errorCode;

    auto descriptor = SocketUtils::createUdpSocket(_bindAddress,
                                                   errorCode);

    if (errorCode != 0 && errorCode != EINPROGRESS)
    {
        doError(errorCode);
    }
    else
    {
        SocketUtils::setBlockingMode(descriptor, false);

        setDescriptor(descriptor);

#ifdef WIN32
        registerEvent();
#endif
    }
    return  descriptor != INVALID_DESCRIPTOR;
}

void HandlerUdpSocket::close()
{
    Descriptor aDescriptor = descriptor();
    if (INVALID_DESCRIPTOR == aDescriptor)
        return;

#ifdef WIN32
    unregisterEvent();
#endif

    SocketUtils::closeSocket(aDescriptor);
    setDescriptor(INVALID_DESCRIPTOR);
}

int HandlerUdpSocket::inputBytesAvailable()
{
    return SocketUtils::inputBytesAvalable(descriptor());
}

int HandlerUdpSocket::outputBytesAvailable()
{
    return -1;
}

#ifdef WIN32

DescriptorsVector *HandlerUdpSocket::events()
{
    auto eventsVector = HandlerBase::events();

    if (descriptor() == INVALID_DESCRIPTOR || eventsVector->size() < 1)
        return eventsVector;

    // событие закрытия сокета принимается всегда
    long mask = 0;

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
    // установка маски на событие сокета
    int operationResult = WSAEventSelect((SOCKET)descriptor(), eventsVector->at(0), mask);
    if (operationResult != 0)
        doError(SocketUtils::lastError());

    return eventsVector;
}

bool HandlerUdpSocket::process(Descriptor eventToProcess)
{
    auto eventsVector = events();
    auto socket = (SOCKET)descriptor();
    if (socket == (SOCKET)INVALID_DESCRIPTOR || eventsVector->size() < 1 ||
            eventsVector->at(0) != eventToProcess)
        return false;

    ResetEvent(eventToProcess);

    WSANETWORKEVENTS NetworkEvents;
    if (WSAEnumNetworkEvents(socket, eventToProcess, &NetworkEvents) != 0)
    {
        doError(SocketUtils::lastError());
        return false;
    }

    // событие подключения
    // событие прихода данных
    if (NetworkEvents.lNetworkEvents & FD_READ)
    {
        if (NetworkEvents.iErrorCode[FD_READ_BIT] == 0)
        {
            // вызов события прихода данных
            doReadyRead();;
        }
        else
        {
            // Произошла ошибка. Вызов события обработки ошибки
            doError(NetworkEvents.iErrorCode[FD_READ_BIT]);
        }
    }

    // событие готовности к отправке данных
    if (NetworkEvents.lNetworkEvents & FD_WRITE)
    {
        if (NetworkEvents.iErrorCode[FD_WRITE_BIT] == 0)
        {
            // вызов события готовности к отправке данных
            doReadyWrite();
        }
        else {
            // Произошла ошибка. Вызов события обработки ошибки
            doError(NetworkEvents.iErrorCode[FD_WRITE_BIT]);
        }
    }

    // ResetEvent(eventToProcess);

    return true;
}

#endif

#ifdef linux

void HandlerUdpSocket::assign(pollfd &event)
{
    auto cs = connectionState();
    if (ConnectionState::Connected != cs)
        return;

    event.fd = descriptor();
    event.revents = 0;
    event.events = POLLIN | POLLERR | POLLHUP | POLLNVAL | POLLRDHUP;
    if (needsToWrite() || ConnectionState::Connecting == cs)
        event.events |= POLLOUT;
}

bool HandlerUdpSocket::process(const pollfd &event)
{
    auto d = descriptor();

    if (ConnectionState::Connected != connectionState()
            || INVALID_DESCRIPTOR == d)
        return false;

    if (event.fd != d)
        return false;

    // если произошла ошибка, то её нужно отработать в первую очередь
    // пока всё ещё живо
    if (event.revents & (POLLERR | POLLNVAL))
    {
        doError(SocketUtils::lastError());
        close();
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
                }
                else
                {
                    doError(SocketUtils::lastError());
                    close();
                    return false;
                };
            }
            else
            {
                doError(SocketUtils::lastError());
                close();
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

#endif

int HandlerUdpSocket::read(void *data, int count)
{
    if (descriptor() == INVALID_DESCRIPTOR)
        return -1;

#ifdef linux
    socklen_t addrSize = sizeof(_readAddress);
    return recvfrom(descriptor(),
                    data,
                    count,
                    0,
                    reinterpret_cast<struct sockaddr*>(&_readAddress),
                    &addrSize);
#endif
#ifdef WIN32
    int addrSize = sizeof(_remoteAddress);
    return recvfrom((SOCKET)descriptor(), (char*)data, count, 0,
                    reinterpret_cast<struct sockaddr*>(&_remoteAddress), &addrSize);
#endif
    return -1;
}

int HandlerUdpSocket::write(const void *data, int count)
{
    if (descriptor() == INVALID_DESCRIPTOR || count > MAXIMUM_DATA_SIZE)
        return -1;

    return writeTo(_remoteAddress, data, count);

}

int HandlerUdpSocket::writeTo(const sockaddr_in &address,
                              const void *data,
                              int count)
{
#ifdef linux
    return sendto(descriptor(),
                  data,
                  (size_t)count,
                  0,
                  reinterpret_cast<struct sockaddr*>(const_cast<struct sockaddr_in*>(&address)),
                  (int)sizeof(address));
#endif
#ifdef WIN32
    return sendto((SOCKET)descriptor(),
                  (char*)data,
                  count,
                  0,
                  reinterpret_cast<struct sockaddr*>(const_cast<struct sockaddr_in*>(&address)),
                  (int)sizeof(address));
#endif
    return -1;
}

int HandlerUdpSocket::writeTo(const std::string &ipAddress,
                              const void *data,
                              int count)
{
    if (descriptor() == INVALID_DESCRIPTOR || count > MAXIMUM_DATA_SIZE)
        return -1;

    sockaddr_in address;
    if (_addressesMap.count(ipAddress) > 0)
    {
        address = _addressesMap[ipAddress];
    }
    else
    {
        address = SocketUtils::ipAddressStringToAddress(ipAddress, _port);
        _addressesMap[ipAddress] = address;
    }

    return writeTo(address, data, count);
}

const sockaddr_in HandlerUdpSocket::remoteAddress() const
{
    return _remoteAddress;
}

const sockaddr_in &HandlerUdpSocket::readAddress() const
{
    return _readAddress;
}

#ifdef WIN32

void HandlerUdpSocket::registerEvent()
{
    unregisterEvent();

    assign({WSACreateEvent()});
}

void HandlerUdpSocket::unregisterEvent()
{
    auto eventsArray = events();
    for (auto e : *eventsArray)
    {
        WSACloseEvent(e);
    }
    assign({});
}

#endif

}}
