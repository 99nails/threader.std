#include "ThreadMainUdpSender.h"

#include "ThreadUdpReceiver.h"

ThreadMainUdpSender::ThreadMainUdpSender(const std::string &applicationBinaryName,
                                         const std::string &applicationFriendlyName)
    : ThreadMainWithouSettings(nullptr, applicationBinaryName, applicationFriendlyName)
    , _threadReceiver(nullptr)
{
}


void ThreadMainUdpSender::startChildThreads()
{
    _threadReceiver = registerAndStartChildThread(new ThreadUdpReceiver(this));
}
