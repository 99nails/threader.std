#pragma once

#include <ThreadConnections.h>

using namespace Threader::Threads;

class ThreadUdpReceiver : public ThreadConnections
{
public:
    explicit ThreadUdpReceiver(MessageConsumer *parent,
                               const std::string bindIpAddress);

    // HandlerDataConsumer interface
public:
    void onError(const ConnectionBase &connection,
                 const int &errorCode,
                 const std::string &errorDescription) override;

    // ThreadBase interface
protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;
    void onIdle() override;

    // MessageConsumer interface
protected:
    void processMessage(const MessageBase::Ptr &) override;

protected:
    void onConnecting(const ConnectionBase &connection) override;
    void onConnected(const ConnectionBase &connection) override;
    void onDisconnected(const ConnectionBase &connection) override;

    void onReadData(const ConnectionBase &connection,
                    const char *,
                    const int &size) override;
    void onWriteData(const ConnectionBase &connection,
                     const char *,
                     const int &size) override;
    void onPacketReceived(const ConnectionBase &connection,
                          const PacketBase::Ptr &) override;
    void onPacketSent(const ConnectionBase &connection,
                      const PacketBase::Ptr &) override;

private:
    std::string _bindIpAddress;
    ConnectionBase *_connectionIn;
    ConnectionBase *_connectionOut;
};

