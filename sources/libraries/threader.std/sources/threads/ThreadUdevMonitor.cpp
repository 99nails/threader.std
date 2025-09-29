#include "ThreadUdevMonitor.h"

namespace Threader::Threads {

ThreadUdevMonitor::ThreadUdevMonitor(MessageConsumer *parent,
                                     const std::vector<std::string> &matchedSubsystems)
    : ThreadBase(parent, "Thread.Udev")
    , _matchedSubsystems(matchedSubsystems)
{
}

void ThreadUdevMonitor::initializeThread()
{
    ThreadBase::initializeThread();

    _poller = new PollerUdev(this, _matchedSubsystems);
    _poller->initialize();
    polling()->registerPoller(_poller);
}

void ThreadUdevMonitor::processDevice(const std::string &action,
                                      const DeviceInfo &deviceInfo)
{
}

}
