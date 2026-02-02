#include "ThreadConnections.h"

namespace Threader::Threads {

ThreadConnections::ThreadConnections(MessageConsumer *parent,
                                     PacketFactoryBase *factory,
                                     const HandlerVector &handlers)
    : ThreadBase(parent)
    , _factory(factory)
{
    registerConnectionHandlers(handlers);
}

ThreadConnections::~ThreadConnections()
{
}

PacketFactoryBase *ThreadConnections::factory() const
{
    return _factory;
}

ConnectionBase* ThreadConnections::registerConnectionHandler(HandlerBase *handler)
{
    auto result = new ConnectionBase(this, handler, 10000, _factory);
    /*
    result->moveToThread(this);
    connect(result, &ConnectionBase::signalOnConnectionStateChanged,
            this, &ThreadConnections::slotOnConnectionStateChanged, Qt::DirectConnection);
    connect(result, &ConnectionBase::signalOnError,
            this, &ThreadConnections::slotOnError, Qt::DirectConnection);

    connect(result, &ConnectionBase::signalOnReadData,
            this, &ThreadConnections::slotOnReadData, Qt::DirectConnection);
    connect(result, &ConnectionBase::signalOnWriteData,
            this, &ThreadConnections::slotOnWriteData, Qt::DirectConnection);

    connect(result, &ConnectionBase::signalOnPacketReceived,
            this, &ThreadConnections::slotOnPacketReceived, Qt::DirectConnection);
    connect(result, &ConnectionBase::signalOnPacketSent,
            this, &ThreadConnections::slotOnPacketSent, Qt::DirectConnection);

    */
    _connections.push_back(result);
    return result;
}

void ThreadConnections::registerConnectionHandlers(const HandlerVector &handlers)
{
    for (const auto &handler: handlers)
        registerConnectionHandler(handler);
}

void ThreadConnections::terminateChildThreads()
{
    ThreadBase::terminateChildThreads();
    for (const auto &connection: _connections)
    {
        if (connection && connection->handler())
            connection->handler()->close();
    }
}

void ThreadConnections::onBeforeWaitEvents()
{
    ThreadBase::onBeforeWaitEvents();

    for (const auto &connection: _connections)
    {
        connection->onBeforeWaitEvents();
    }
}

ConnectionList ThreadConnections::connections() const
{
    return _connections;
}

void ThreadConnections::onConnectionStateChanged(ConnectionBase *connection,
                                                     HandlerBase::ConnectionState oldState,
                                                     HandlerBase::ConnectionState newState)
{
    if (!connection || !connection->handler())
        return;

    switch (newState) {
    case HandlerBase::ConnectionState::Connecting:
        onConnecting(connection);
        break;
    case HandlerBase::ConnectionState::Connected:
        onConnected(connection);
        break;
    case HandlerBase::ConnectionState::Disconnected:
        if (HandlerBase::ConnectionState::Connected == oldState)
        onDisconnected(connection);
        break;
    }
}

void ThreadConnections::onError(ConnectionBase *connection,
                                    const int &errorCode,
                                    const std::string &errorDescription)
{
    if (!connection || !connection->handler())
        return;

    onError(connection, errorCode, errorDescription);
}

void ThreadConnections::onReadData(ConnectionBase *connection,
                                       const char *data,
                                       const int &size)
{
    if (!connection || !connection->handler())
        return;
    onReadData(connection, data, size);
}

void ThreadConnections::onWriteData(ConnectionBase *connection,
                                        const char *data,
                                        const int &size)
{
    if (!connection || !connection->handler())
        return;

    onWriteData(connection, data, size);
}

void ThreadConnections::onPacketReceived(ConnectionBase *connection,
                                             const PacketBase::Ptr &packet)
{
    onPacketReceived(connection, packet);
}

void ThreadConnections::onPacketSent(ConnectionBase *connection,
                                         const PacketBase::Ptr &packet)
{
    onPacketSent(connection, packet);
}

}
