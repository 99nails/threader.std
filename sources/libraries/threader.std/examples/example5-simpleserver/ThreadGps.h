#pragma once

#include "SettingsGps.h"

#include "ThreadHandler.h"

using namespace Threader::Threads;

class ThreadGps : public ThreadHandler
{
public:
    explicit ThreadGps(const SettingsGps::Ptr &settings,
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
    SettingsGps::Ptr _settings;

private:
//    void sendGpsData();

    void doGpsCommandGNZDA(const StdStringVector &words);

};
