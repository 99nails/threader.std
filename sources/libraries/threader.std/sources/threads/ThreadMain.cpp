#include "ThreadMain.h"

#include "ListThreads.h"
#include "MessageWriteToFile.h"
#include "SignalHandler.h"
#include "StdStringUtils.h"

#ifdef WIN32
#include <psapi.h>
#endif

namespace Threader::Threads
{

ThreadMainWithoutSettings::ThreadMainWithoutSettings(MessageConsumer *parent,
                                                   const std::string &applicationBinaryName,
                                                   const std::string &applicationFriendlyName,
                                                   const std::string &threadName)
    : ThreadBase(parent,
                 (threadName.empty()) ? "Thread.Main" : threadName)
    , _applicationBinaryName(applicationBinaryName)
    , _applicationFriendlyName(applicationFriendlyName)
    , _nextBuildStatisticTickCount(0)
    , _nextBuildCommonStatisticTickCount(0)
    , _threadStatisticFileName(applicationBinaryName + ".threads")
{
    if (_applicationFriendlyName.empty())
        _applicationFriendlyName = threadName;

    ThreadsStaticticCollector::setThreadStatisticFileName(_threadStatisticFileName);
}

void ThreadMainWithoutSettings::onThreadStarted()
{
    writeLog(Message10, _applicationFriendlyName.c_str());

    snapshotThreads(true);
}

void ThreadMainWithoutSettings::onThreadFinishing()
{
    writeLog(Message11, _applicationFriendlyName.c_str());
}

void ThreadMainWithoutSettings::onThreadFinished()
{
    writeLog(Message12, _applicationFriendlyName.c_str());

    snapshotThreads(true);
}

void ThreadMainWithoutSettings::onIdle()
{
    snapshotThreads();
}

void ThreadMainWithoutSettings::snapshotThreads(bool rightNow)
{
    auto tickCount = DateTimeUtils::getTickCount();
    if (!rightNow && tickCount < _nextBuildStatisticTickCount)
        return;

    _nextBuildStatisticTickCount =
            DateTimeUtils::getNextTickCount(TIMEOUT_BUILD_STATISTIC_MILLISECONDS);

    accumulateStatistic(rightNow);

    auto listThreads = ThreadsStaticticCollector::instance();
    if (!listThreads)
        return;

    listThreads->sendSnapshot();
}

const std::string &ThreadMainWithoutSettings::applicationFriendlyName() const
{
    return _applicationFriendlyName;
}

const std::string &ThreadMainWithoutSettings::applicationBinaryName() const
{
    return _applicationBinaryName;
}

}
