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

    virtual void onConnecting(const ConnectionBase&){}
    virtual void onConnected(const ConnectionBase&){}
    virtual void onDisconnected(const ConnectionBase&){}

private:
    PacketFactoryBase *_factory;
    ConnectionList _connections;

public:
    void onConnectionStateChanged(const ConnectionBase &connection,
                                  const HandlerBase::ConnectionState oldState,
                                  const HandlerBase::ConnectionState newState) override;

    void onError(const ConnectionBase &connection,
                 const int &errorCode,
                 const std::string &errorDescription) override;

    void onReadData(const ConnectionBase &connection,
                    const char *data,
                    const int &size) override;

    void onWriteData(const ConnectionBase &connection,
                     const char *data,
                     const int &size) override;

    void onPacketReceived(const ConnectionBase &connection,
                          const PacketBase::Ptr &packet) override;

    void onPacketSent(const ConnectionBase &connection,
                      const PacketBase::Ptr &packet) override;
};

}
