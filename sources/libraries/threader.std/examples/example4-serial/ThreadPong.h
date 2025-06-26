#pragma once

#include <ThreadHandler.h>

using namespace Threader::Threads;

class ThreadPong : public ThreadHandler
{
public:
    explicit ThreadPong(const std::string &deviceName);

protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;

    // ThreadHandler interface
protected:
    void onConnecting() override;
    void onConnected() override;
    void onDisconnected() override;
    void onReadData(const void *, const int &count) override;
    void onWriteData(const void *, const int &count) override;
    void onError(HandlerBase *handler,
                 const int errorCode,
                 const std::string &errorDescription) override;

    bool onPacketReceived(const PacketBase::Ptr &packet) override;
    bool onPacketSent(const PacketBase::Ptr &packet) override;
};
