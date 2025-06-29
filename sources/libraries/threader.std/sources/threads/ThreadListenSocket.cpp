#include "ThreadListenSocket.h"

#include "DateTimeUtils.h"
#include "SocketUtils.h"

//#include "../Utils/BackwardCompatibility.h"

namespace Threader {

namespace Threads {


using namespace Threader::Utils;


const uint ThreadListenSocket::TIMEOUT_REOPEN_LISTEN_SOCKET_MILLISECONDS = 30000;


ThreadListenSocket::ThreadListenSocket(uint16_t port,
                                       MessageConsumer *parent)
    : ThreadBase(parent)
    , _ports({port})
    , _portsString(std::to_string(port))
    , _nextStartListening(DateTimeUtils::getTickCount())
{
    registerListener(new PollerListenSocket(this, port));
}

ThreadListenSocket::ThreadListenSocket(const PortVector &ports, MessageConsumer *parent)
    : ThreadBase(parent)
    , _nextStartListening(DateTimeUtils::getTickCount())
{
    for (auto port : ports)
    {
        if (port == 0)
            continue;

        _ports.push_back(port);

        registerListener(new PollerListenSocket(this, port));
        if (!_portsString.empty())
            _portsString +=", ";
        _portsString += std::to_string(port);
    }
}

ThreadListenSocket::~ThreadListenSocket()
{
}

uint16_t ThreadListenSocket::port(int index) const
{
    return _ports.at(index);
}

PollerListenSocket *ThreadListenSocket::listeners(int index) const
{
    return _listeners.at(index);
}

void ThreadListenSocket::registerListener(PollerListenSocket *listener)
{
    if (!listener)
        return;

//    listener->moveToThread(this);
//    connect(listener, &PollerListenSocket::signalOnListenSocketError,
//            this, &ThreadListenSocket::slotOnListenSocketError, Qt::DirectConnection);
//    connect(listener, &PollerListenSocket::signalConnectionRequest,
//            this, &ThreadListenSocket::slotOnConnectionRequest, Qt::DirectConnection);
    _listeners.push_back(listener);
}

std::string ThreadListenSocket::portString()
{
    return _portsString;
}

void ThreadListenSocket::onBeforeWaitEvents()
{
    if (_nextStartListening > DateTimeUtils::getTickCount())
        return;

    for (auto listener: _listeners)
    {
        // проверка необходимости инициализации слушающего сокета
        if (listener->isInitialized())
            continue;;

        // вызов заглушки перед стартом инициализации
        onBeforeListenSocketInitialization(listener);

        // инициализация слущающего сокета
        if (listener->initialize())
        {
            // регистрация в списке голосующих
            polling()->registerPoller(listener);
            // вызов заглушки корректной ининциализации
            onListenSocketInitialized(listener);
        }
        else
        {
            // формирование времени следующего срабатывания ининциализации
            _nextStartListening = DateTimeUtils::getNextTickCount(TIMEOUT_REOPEN_LISTEN_SOCKET_MILLISECONDS);
            // обработчик ошибок вызывается голосующим
        }
    }
}

void ThreadListenSocket::terminateChildThreads()
{
    for (auto listener: _listeners)
    {
        listener->finalize();
    }
    ThreadBase::terminateChildThreads();
}

//void ThreadListenSocket::onListenSocketError(PollerListenSocket *listener,
//                                             int errorCode)
//{

//}

void ThreadListenSocket::slotOnListenSocketError(PollerListenSocket *sender,
                                                 int errorCode)
{
    onListenSocketError(sender, errorCode);
}

void ThreadListenSocket::slotOnConnectionRequest(PollerListenSocket *sender,
                                                 Descriptor socket,
                                                 const sockaddr_in &addr,
                                                 bool &accept)
{
//    std::string ipAddress = SocketUtils::sockAddrToString(addr);
//    onAcceptConnectionRequest(sender, socket, ipAddress, accept);
}

}}
