#include "PollerListenSocketConnection.h"

#include "SocketUtils.h"

namespace Threader::Threads
{


using namespace Threader::Utils;


PollerListenSocketConnection::PollerListenSocketConnection(ConsumerListenSocketEvents *consumer,
                                       uint16_t port)
    : PollerBase()
    , _isInitialized(false)
    , _port(port)
    , _consumer(consumer)
    #ifdef WIN32
    , _descriptor(INVALID_DESCRIPTOR)
    , _event(INVALID_DESCRIPTOR)
    #endif
{
}

PollerListenSocketConnection::~PollerListenSocketConnection()
{

}

bool PollerListenSocketConnection::isInitialized() const
{
    return _isInitialized;
}

uint16_t PollerListenSocketConnection::port() const
{
    return _port;
}

bool PollerListenSocketConnection::initialize()
{
    if (_isInitialized)
        return true;

    sockaddr_in addr;
    int errorCode;

    setDescriptor(SocketUtils::createTcpServerSocket(_port,
                                                     false,
                                                     addr,
                                                     errorCode));

    bool result = (errorCode == 0);
    if (!result)
    {
        if (_consumer)
            _consumer->onListenSocketError(this, errorCode);
        _isInitialized = false;
        return false;
    }
#ifdef linux
    setEvents(POLLIN | POLLERR);
#endif
#ifdef WIN32
    _event = WSACreateEvent();
    if (WSAEventSelect((SOCKET)_descriptor, _event, FD_ACCEPT) != 0)
    {
        if (_consumer)
            _consumer->onListenSocketError(this, errorCode);
        _isInitialized = false;
        return false;
    }

    assign({_event});
#endif
    _isInitialized = true;
    return result;
}

void PollerListenSocketConnection::finalize()
{
#ifdef WIN32
    finalizeWindows();
#endif

#ifdef linux
    finalizeLinux();
#endif
    _isInitialized = false;
}

#ifdef WIN32

Descriptor PollerListenSocketConnection::descriptor() const
{
    return _descriptor;
}

void PollerListenSocketConnection::setDescriptor(const Descriptor descriptor)
{
    _descriptor = descriptor;
}

void PollerListenSocketConnection::finalizeWindows()
{
    if (INVALID_SOCKET == (SOCKET)_descriptor)
        return;

    closesocket((SOCKET)_descriptor);
    _descriptor = (Descriptor)INVALID_SOCKET;
}

bool PollerListenSocketConnection::process(Descriptor eventToProcess)
{
    if (_event != eventToProcess)
        return false;

    ResetEvent(eventToProcess);

    sockaddr_in clientAddress;
    int clientAddressLength = sizeof(clientAddress);
    auto newSocket = WSAAccept((SOCKET)_descriptor,
                               (SOCKADDR*) &clientAddress,
                               &clientAddressLength,
                               nullptr, 0);

    if (INVALID_SOCKET == newSocket)
    {
        if (_consumer)
            _consumer->onListenSocketError(this, SocketUtils::lastError());
        return true;
    }

    bool acceptConnection = true;
    if (_consumer)
        _consumer->onConnectionRequest(this,
                                       Descriptor(newSocket),
                                       clientAddress,
                                       acceptConnection);

    if (!acceptConnection)
        SocketUtils::closeSocket((Descriptor)newSocket);
    return true;
}

#endif

#ifdef linux

void PollerListenSocketConnection::finalizeLinux()
{
    if (descriptor() == INVALID_DESCRIPTOR)
        return;
    SocketUtils::closeSocket(descriptor());
    setDescriptor(INVALID_DESCRIPTOR);
    setEvents(0);
}

bool PollerListenSocketConnection::process(const pollfd &event)
{
    if (descriptor() != event.fd)
        return false;

    if (event.revents & POLLERR)
        finalize();

    if (event.revents & POLLIN)
        processConnectionRequests();

    return true;
}

int PollerListenSocketConnection::processConnectionRequests()
{
    int result = 0;
    Descriptor newSocket;
    sockaddr_in clientAddress;
    do
    {
        socklen_t clientAddressSize = sizeof(clientAddress);
        newSocket = accept(descriptor(), reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressSize);

        if (INVALID_DESCRIPTOR == newSocket)
        {
            int errorCode = SocketUtils::lastError();
            if (errorCode != EWOULDBLOCK)
            {
                result++;
                if (_consumer)
                    _consumer->onListenSocketError(this, errorCode);
            }
            break;
        }
        else
        {
            result++;
            bool acceptConnection = true;

            SocketUtils::setBlockingMode(newSocket, false);

            if (_consumer)
                _consumer->onConnectionRequest(this,
                                         newSocket,
                                         clientAddress,
                                         acceptConnection);
            if (!acceptConnection)
                SocketUtils::closeSocket(newSocket);
        }
    }
    while (newSocket != INVALID_DESCRIPTOR);

    return result;
}

#endif

}
