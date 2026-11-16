#include "ThreadMainQt.h"

#ifdef linux
#include <sys/resource.h>
#endif

#ifdef WIN32
#include <psapi.h>
#endif


namespace Threader::Threads::QtEventLoop
{

ThreadMainQt::ThreadMainQt(MessageConsumer *parent,
                           const std::string &applicationBinaryName,
                           const std::string &applicationFriendlyName,
                           const std::string &threadName)
    : ThreadQtEventLoop(parent,
                        (threadName.empty()) ? "Thread.Main" : threadName)
    , _applicationBinaryName(applicationBinaryName)
    , _applicationFriendlyName(applicationFriendlyName)
    , _nextBuildStatisticTickCount(
          DateTimeUtils::getNextTickCount(TIMEOUT_BUILD_STATISTIC_MILLISECONDS))
    , _nextBuildCommonStatisticTickCount(0)
    , _threadStatisticFileName(applicationBinaryName + ".Threads")
{
    if (_applicationFriendlyName.empty())
        _applicationFriendlyName = threadName;

    ThreadsStaticticCollector::setThreadStatisticFileName(_threadStatisticFileName);
}

const std::string &ThreadMainQt::applicationFriendlyName() const
{
    return _applicationFriendlyName;
}

const std::string &ThreadMainQt::applicationBinaryName() const
{
    return _applicationBinaryName;
}

void ThreadMainQt::onThreadStarted()
{
    writeLog(Message10, _applicationFriendlyName.c_str());

    snapshotThreads(true);
}

void ThreadMainQt::onThreadFinishing()
{
    writeLog(Message11, _applicationFriendlyName.c_str());
}

void ThreadMainQt::onThreadFinished()
{
    writeLog(Message12, _applicationFriendlyName.c_str());

    snapshotThreads(true);
}

void ThreadMainQt::onIdle()
{
    snapshotThreads();
}

void ThreadMainQt::snapshotThreads(bool rightNow)
{
    auto tickCount = DateTimeUtils::getTickCount();
    if (!rightNow && tickCount < _nextBuildStatisticTickCount)
        return;

    _nextBuildStatisticTickCount =
        DateTimeUtils::getNextTickCount(TIMEOUT_BUILD_STATISTIC_MILLISECONDS);

    auto listThreads = ThreadsStaticticCollector::instance();
    if (!listThreads)
        return;

    listThreads->sendSnapshot();
}

}
