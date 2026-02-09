#include "ThreadMainUdpSender.h"

#include "ThreadUdpReceiver.h"
#include "SocketUtils.h"

ThreadMainUdpSender::ThreadMainUdpSender(const std::string &applicationBinaryName,
                                         const std::string &applicationFriendlyName)
    : ThreadMainWithouSettings(nullptr,
                               applicationBinaryName,
                               applicationFriendlyName)
    , _threadReceiver(nullptr)
{
}


void ThreadMainUdpSender::startChildThreads()
{
    auto info = SocketUtils::interfaceInfo("tap0", AF_INET);
    if (!info.ipAddress.empty())
    {
        _threadReceiver = registerAndStartChildThread(new ThreadUdpReceiver(this,
                                                                            info.ipAddress));
    }
}
