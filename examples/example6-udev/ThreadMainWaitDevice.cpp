#include "ThreadMainWaitDevice.h"

#include <ThreadUdevMonitor.h>

ThreadMainWaitDevice::ThreadMainWaitDevice(const std::string &applicationName,
                                           const std::string &description)
    : ThreadMain(nullptr,
                 applicationName,
                 description,
                 "Thread.Main",
                 format("%s%c%s", SETTINGS_FOLDER, FileUtils::DELIMITER, "example6.ini"))
    , _threadUdevMonitor(nullptr)
{
}

void ThreadMainWaitDevice::startChildThreads()
{
    _threadUdevMonitor = registerAndStartChildThread(new ThreadUdevMonitor(this, {"tty"}));
}
