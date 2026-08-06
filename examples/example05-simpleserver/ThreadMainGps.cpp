#include "ThreadMainGps.h"

#include "Example05Common.h"
#include "ThreadGpsEmulator.h"
#include "StdStringUtils.h"

using namespace Threader::Utils;

constexpr char THREAD_NAME[] = "Thread.Main.Time.Server";

ThreadMainGps::ThreadMainGps(const std::string &applicationName,
                             const std::string &description)
    : ThreadMain(nullptr,
                 applicationName,
                 description,
                 THREAD_NAME,
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
