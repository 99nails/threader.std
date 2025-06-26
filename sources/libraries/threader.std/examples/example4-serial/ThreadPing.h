#pragma once

#include <ThreadHandler.h>

using namespace Threader::Threads;

class ThreadPing : public ThreadHandler
{
public:
    explicit ThreadPing(const std::string &deviceName);

protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;
    void processMessage(const MessageBase::Ptr &message) override;

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

private:

};
