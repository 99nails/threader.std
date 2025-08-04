#include "ThreadMainGps.h"

#include "Example5Common.h"
#include "ThreadGpsEmulator.h"

constexpr char APPLICATION_NAME[] = "Gps.Emulator";

ThreadMainGps::ThreadMainGps()
    : ThreadBase(nullptr, "Thread.Main")
    , _threadGps(nullptr)
    , _settings(std::make_shared<SettingsExample5>())
{
}

void ThreadMainGps::onThreadStarted()
{
    writeLog(Message10, APPLICATION_NAME);
    _settings->read();
}

void ThreadMainGps::onThreadFinishing()
{
    writeLog(Message11, APPLICATION_NAME);
}

void ThreadMainGps::onThreadFinished()
{
    writeLog(Message12, APPLICATION_NAME);
}

void ThreadMainGps::startChildThreads()
{
    auto settingsGps = _settings->get<SettingsGpsEmulator>();
    _threadGps = registerAndStartChildThread(new ThreadGpsEmulator(settingsGps.get(),
                                                                   this));
}
