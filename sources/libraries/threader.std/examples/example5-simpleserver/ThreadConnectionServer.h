#pragma once

#include "ThreadHandler.h"

using namespace Threader::Threads;

class ThreadConnectionServer : public ThreadHandler
{
public:
    explicit ThreadConnectionServer(MessageConsumer *parent,
                                    const std::string &host,
                                    const uint16_t port,
                                    const Descriptor descriptor);

    // HandlerDataConsumer interface
public:
    void onError(HandlerBase *sender, const int errorCode, const std::string &errorDescription) override;

    // ThreadHandler interface
protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;

    void onConnected() override;
    void onDisconnected() override;

    void onReadData(const void *, const int &count) override;
    void onWriteData(const void *, const int &count) override;

    bool onPacketReceived(const PacketBase::Ptr &packet) override;
    bool onPacketSent(const PacketBase::Ptr &packet) override;
};

