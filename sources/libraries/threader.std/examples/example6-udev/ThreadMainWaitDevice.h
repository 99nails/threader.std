#pragma once

#include "SettingsDevice.h"

#include <ThreadMain.h>
using namespace Threader::Threads;

using SettingsType = SettingsBundleTemplate<SettingsDevice>;

class ThreadMainWaitDevice : public ThreadMain<SettingsType>
{
public:
    explicit ThreadMainWaitDevice(const std::string &applicationName);

protected:
    void startChildThreads() override;

private:
    ThreadBase *_threadUdevMonitor;
};

