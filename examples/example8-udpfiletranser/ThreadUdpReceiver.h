#pragma once

#include <ThreadHandler.h>

using namespace Threader::Threads;

class ThreadUdpReceiver : public ThreadHandler
{
public:
    explicit ThreadUdpReceiver(MessageConsumer *parent);

    // HandlerDataConsumer interface
public:
    void onError(HandlerBase *sender,
                 const int errorCode,
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
    void onConnecting() override;
    void onConnected() override;
    void onDisconnected() override;
    void onReadData(const void *, const int &) override;
    void onWriteData(const void *, const int &) override;
    bool onPacketReceived(const PacketBase::Ptr &) override;
    bool onPacketSent(const PacketBase::Ptr &) override;
};

