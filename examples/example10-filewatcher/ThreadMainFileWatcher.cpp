#include "ThreadMainFileWatcher.h"

#include "Logger.h"
#include "ThreadFileWatcher.h"

using namespace Threader::Utils;

ThreadMainFileWatcher::ThreadMainFileWatcher(const std::string &applicationBinaryName,
                                 const std::string &applicationFriendlyName)
    : ThreadMainWithouSettings(nullptr,
                               applicationBinaryName,
                               applicationFriendlyName)
{
    setTimeout(1000);
}

void ThreadMainFileWatcher::startChildThreads()
{
//    auto homePath = std::getenv("HOME");
    auto homePath = "/var/log/";
    _threadFileNotifier = registerAndStartChildThread(new ThreadFileWatcher(this, homePath));
}
