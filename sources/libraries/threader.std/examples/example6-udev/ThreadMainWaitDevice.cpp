#include "ThreadMainWaitDevice.h"

#include <ThreadUdevMonitor.h>

ThreadMainWaitDevice::ThreadMainWaitDevice(const std::string &applicationName)
    : ThreadMain(nullptr,
                 applicationName,
                 "Thread.Main",
                 format("%s%c%s", SETTINGS_FOLDER, FileUtils::DELIMITER, "example6.ini"))
    , _threadUdevMonitor(nullptr)
{
}


void ThreadMainWaitDevice::startChildThreads()
{
    _threadUdevMonitor = registerAndStartChildThread(new ThreadUdevMonitor(this, {"tty"}));
}
