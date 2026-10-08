#include "ThreadListenSocketConnection.h"

#include "DateTimeUtils.h"
#include "SocketUtils.h"

namespace Threader {

namespace Threads {


using namespace Threader::Utils;


const uint32_t ThreadListenSocketConnection::TIMEOUT_REOPEN_LISTEN_SOCKET_MILLISECONDS = 30000;


ThreadListenSocketConnection::ThreadListenSocketConnection(uint16_t port,
                                                           MessageConsumer *parent)
    : ThreadBase(parent, "Thread.Listen")
    , _ports({port})
    , _portsString(std::to_string(port))
    , _nextStartListening(DateTimeUtils::getTickCount())
{
    registerListener(new PollerListenSocketConnection(this, port));
}

ThreadListenSocketConnection::ThreadListenSocketConnection(const PortVector &ports,
                                                           MessageConsumer *parent)
    : ThreadBase(parent, "Thread.Listen")
    , _nextStartListening(DateTimeUtils::getTickCount())
{
    for (auto port : ports)
    {
        if (port == 0)
            continue;

        _ports.push_back(port);

        registerListener(new PollerListenSocketConnection(this, port));
        if (!_portsString.empty())
            _portsString +=", ";
        _portsString += std::to_string(port);
    }
}

ThreadListenSocketConnection::~ThreadListenSocketConnection()
{
}

uint16_t ThreadListenSocketConnection::port(int index) const
{
    return _ports.at(index);
}

PollerListenSocketConnection *ThreadListenSocketConnection::listeners(int index) const
{
    return _listeners.at(index);
}

void ThreadListenSocketConnection::registerListener(PollerListenSocketConnection *listener)
{
    if (!listener)
        return;

    _listeners.push_back(listener);
}

std::string ThreadListenSocketConnection::portString()
{
    return _portsString;
}

void ThreadListenSocketConnection::onBeforeWaitEvents()
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

void ThreadListenSocketConnection::terminateChildThreads()
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

void ThreadListenSocketConnection::slotOnListenSocketError(PollerListenSocketConnection *sender,
                                                           int errorCode)
{
    onListenSocketError(sender, errorCode);
}

void ThreadListenSocketConnection::slotOnConnectionRequest(PollerListenSocketConnection *sender,
                                                           Descriptor socket,
                                                           const sockaddr_in &addr,
                                                           bool &accept)
{
    //    std::string ipAddress = SocketUtils::sockAddrToString(addr);
    //    onAcceptConnectionRequest(sender, socket, ipAddress, accept);
}

}}
