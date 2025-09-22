#pragma once

#include "SettingsTimeServer.h"

#include <ThreadHandler.h>

using namespace Threader::Threads;

class ThreadConnectionClient : public ThreadHandler
{
public:
    explicit ThreadConnectionClient(MessageConsumer *parent,
                                    const SettingsTimeServer::Ptr &settings);

    // HandlerDataConsumer interface
public:
    void onError(HandlerBase *sender,
                 const int errorCode,
                 const std::string &errorDescription) override;

private:
    SettingsTimeServer::Ptr _settings;
    long _gmtOffset;

    // ThreadBase interface
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
    bool onPacketReceived(const PacketBase::Ptr &packet) override;
    bool onPacketSent(const PacketBase::Ptr &packet) override;
};

