#pragma once

#include "Example5Common.h"
#include "SettingsGps.h"
#include "SettingsTimeServer.h"

#include "ThreadMain.h"

using namespace Threader::Threads;

using SettingsType = SettingsBundleTemplate<SettingsGps, SettingsTimeServer>;

using MapThreadBase = std::map<ThreadBase*, UnixTimeMSecs>;

class ThreadMainTimeServer : public ThreadMain<SettingsType>
{
public:
    explicit ThreadMainTimeServer(const std::string &applicationName,
                                  const std::string &description);

protected:
    void startChildThreads() override;

    void onDestroyTerminatedChildThread(ThreadBase *thread) override;

    void processMessage(const MessageBase::Ptr &message) override;

private:
    ThreadBase *_threadGps;
    ThreadBase *_threadListen;

    ListThreadBase _threads;

    void doMessageThread(const MessageThread::Ptr &message);
    void doMessageDateTime(const MessageString::Ptr &message);

};

