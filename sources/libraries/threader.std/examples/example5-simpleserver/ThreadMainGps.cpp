#include "ThreadMainGps.h"

#include "Example5Common.h"
#include "ThreadGpsEmulator.h"
#include "StdStringUtils.h"

using namespace Threader::Utils;

constexpr char APPLICATION_NAME[] = "Gps.Emulator";

ThreadMainGps::ThreadMainGps()
    : ThreadMain(nullptr,
                 "Thread.Main",
                 format("%s%c%s", SETTINGS_FOLDER, FileUtils::DELIMITER, "example5.ini"))
    , _threadGps(nullptr)
{
}

void ThreadMainGps::startChildThreads()
{
    auto settingsGps = settings().get<SettingsGpsEmulator>();
    _threadGps = registerAndStartChildThread(new ThreadGpsEmulator(settingsGps.get(),
                                                                   this));
}
