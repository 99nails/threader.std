#pragma once

#include "SettingsExample5.h"

#include "ThreadBase.h"

// #include "IniFile.h"

using namespace Threader::Threads;


class ThreadMainGps : public ThreadBase
{
public:
    explicit ThreadMainGps();

protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;
    void startChildThreads() override;

private:
    IniFile _iniFile;
    ThreadBase *_threadGps;

    SettingsExample5::Ptr _settings;
};
