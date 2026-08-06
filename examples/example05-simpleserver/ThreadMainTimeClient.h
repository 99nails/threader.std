#pragma once

#include "Example05Common.h"
#include "SettingsGps.h"
#include "SettingsTimeServer.h"

#include "ThreadMain.h"

using namespace Threader::Threads;

using SettingsType = SettingsBundleTemplate<SettingsTimeServer>;

class ThreadMainTimeClient : public ThreadMain<SettingsType>
{
public:
    explicit ThreadMainTimeClient(const std::string &applicationName,
                                  const std::string &description);

protected:
    void startChildThreads() override;

    void processMessage(const MessageBase::Ptr &message) override;

private:
    ThreadBase *_threadClient;

    void doMessageThread(const MessageThread::Ptr &message);
    void doMessageDateTime(const MessageString::Ptr &message);

};

