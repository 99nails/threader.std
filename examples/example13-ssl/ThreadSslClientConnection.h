#pragma once

#include "PacketFactoryAsciiLines.h"
#include "ThreadHandler.h"

using namespace Threader::Threads;

class ThreadSslClientConnection : public ThreadHandler
{
public:
    explicit ThreadSslClientConnection(MessageConsumer *parent,
                                    const std::string &host,
                                    const uint16_t port,
                                    const bool useSsl);

public:
    void onError(HandlerBase &sender,
                 const int errorCode,
                 const std::string &errorDescription) override;

protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;

    void onIdle() override;

    void onConnecting(const HandlerBase &handler) override;
    void onConnected(const HandlerBase &handler) override;
    void onDisconnected(const HandlerBase &handler) override;

    bool onPacketReceived(const PacketBase::Ptr &packet) override;
    bool onPacketSent(const PacketBase::Ptr &packet) override;

private:
    bool _useSsl;
    uint32_t _count;
    PacketFactoryAsciiLines *_factory;
};
