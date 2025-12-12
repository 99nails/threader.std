#pragma once

#include "SettingsGpsEmulator.h"
#include "ThreadMain.h"

using namespace Threader::Threads;

using SettingsType = SettingsBundleTemplate<SettingsGpsEmulator>;

class ThreadMainGps : public ThreadMain<SettingsType>
{
public:
    explicit ThreadMainGps(const std::string &applicationName,
                           const std::string &description);

protected:
    void startChildThreads() override;

private:
    IniFile _iniFile;
    ThreadBase *_threadGps;
};
