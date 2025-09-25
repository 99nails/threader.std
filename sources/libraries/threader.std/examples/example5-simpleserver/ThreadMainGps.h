#pragma once

#include "SettingsGpsEmulator.h"
#include "ThreadBase.h"
#include "ThreadMain.h"

using namespace Threader::Threads;

using SettingsType = SettingsBundleTemplate<SettingsGpsEmulator>;

class ThreadMainGps : public ThreadMain<SettingsType>
{
public:
    explicit ThreadMainGps(const std::string &applicationName = std::string());

protected:
    void startChildThreads() override;

private:
    IniFile _iniFile;
    ThreadBase *_threadGps;
};
