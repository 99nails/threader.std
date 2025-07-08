#pragma once

#include "SettingsGpsEmulator.h"

#include "ThreadHandler.h"

using namespace Threader::Threads;

class ThreadGpsEmulator : public ThreadHandler
{
public:
    explicit ThreadGpsEmulator(SettingsGpsEmulator *settings,
                               MessageConsumer *parent);

protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;

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

    void processMessage(const MessageBase::Ptr &message) override;

private:
    SettingsGpsEmulator *_settings;

private:
    void sendGpsData();
};
