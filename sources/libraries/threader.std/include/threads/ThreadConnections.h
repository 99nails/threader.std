#pragma once

#include "Connection.h"
#include "HandlerBase.h"
#include "PacketFactoryBase.h"
#include "ThreadBase.h"

#include "PacketFactoryBase.h"

namespace Threader::Threads {


using namespace Threader::Utils;


class ThreadConnections : public ThreadBase,
                          public ConnectionDataConsumer
{
public:
    explicit ThreadConnections(MessageConsumer *parent,
                               PacketFactoryBase *factory,
                               const HandlerVector &handlers = {});
    ~ThreadConnections() override;

    PacketFactoryBase *factory() const;

    ConnectionList connections() const;

protected:
    ConnectionBase *registerConnectionHandler(HandlerBase *handler);
    void registerConnectionHandlers(const HandlerVector &handlers);

    void terminateChildThreads() override;
    void onBeforeWaitEvents() override;

    virtual void onConnecting(ConnectionBase*){}
    virtual void onConnected(ConnectionBase*){}
    virtual void onDisconnected(ConnectionBase*){}

private:
    PacketFactoryBase *_factory;
    ConnectionList _connections;

public:
    void onConnectionStateChanged(Threader::Threads::ConnectionBase *connection,
                                      HandlerBase::ConnectionState oldState,
                                      HandlerBase::ConnectionState newState) override;

    void onError(Threader::Threads::ConnectionBase *connection,
                     const int &errorCode,
                     const std::string &errorDescription) override;

    void onReadData(Threader::Threads::ConnectionBase *connection,
                        const char *data, const int &size) override;

    void onWriteData(Threader::Threads::ConnectionBase *connection,
                         const char *data, const int &size) override;

    void onPacketReceived(Threader::Threads::ConnectionBase *connection,
                              const PacketBase::Ptr &packet) override;

    void onPacketSent(Threader::Threads::ConnectionBase *connection,
                          const PacketBase::Ptr &packet) override;
};

}
