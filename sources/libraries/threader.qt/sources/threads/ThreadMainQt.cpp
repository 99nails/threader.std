#include "ThreadMainQt.h"

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
}

void ThreadMainQt::onThreadStarted()
{
    writeLog(Message10, _applicationFriendlyName.c_str());
}

void ThreadMainQt::onThreadFinishing()
{
    writeLog(Message11, _applicationFriendlyName.c_str());
}

void ThreadMainQt::onThreadFinished()
{
    writeLog(Message12, _applicationFriendlyName.c_str());
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

    if (!logThread())
        return;

    auto statisticList = listThreads->statistic();

    std::string statisticString = "";

    int memoryUsage = 0;
#ifdef linux
    rusage ru;
    if (0 == getrusage(RUSAGE_SELF, &ru))
    {
        memoryUsage = ru.ru_maxrss;
        statisticString = format("Memory usage: %d Kb\r\n", ru.ru_maxrss);
    }
#endif

#ifdef WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        memoryUsage = pmc.WorkingSetSize / 1024;
        statisticString = format("Memory usage: %1 Kb\r\n", memoryUsage);
    }
#endif

    int i = 0;
    for (auto iterator = statisticList.cbegin(); iterator != statisticList.cend(); iterator++)
    {
        ThreadStatisticStruct statistic = *iterator;
        int64_t workingTotalMSecs = (statistic.AliveMsecsSinceEpoch - statistic.StartedMsecsSinceEpoch);
        int64_t workingMSecs = workingTotalMSecs - statistic.WaitMSecsCount;

        std::string alive = DateTimeUtils::msecsSinceEpochToStdString(statistic.AliveMsecsSinceEpoch,
                                                                      true,
                                                                      false);

        statisticString += format("%d. (Id: %ld) %s %s (%lld/%lld) [%llu] Queue: %d",
                                  ++i,
                                  statistic.ThreadId,
                                  statistic.ThreadName.c_str(),
                                  alive.c_str(),
                                  workingMSecs,
                                  workingTotalMSecs,
                                  statistic.AliveTickCount,
                                  statistic.QueueCount) +
                           ((statistic.Terminated) ? " Terminated" : "") + "\r\n";

    }

#ifdef linux
    setThreadInfo(statisticString);
#endif

    statisticString += "{EOF}" + std::string(DateTimeUtils::msecsSinceEpochLocal() % 8, ' ');

    ThreadBase *logThreadInstance = logThread();
    if (logThreadInstance)
    {
        logThreadInstance->postMessage1<MessageWriteToFile>(_threadStatisticFileName,
                                                            statisticString,
                                                            MessageWriteToFile::WriteMode::Truncate);
    }

    if (rightNow || _nextBuildCommonStatisticTickCount <= tickCount)
    {
        writeLog(Message15, i, memoryUsage);

        _nextBuildCommonStatisticTickCount = DateTimeUtils::getNextTickCount(TIMEOUT_BUILD_COMMON_STATISTIC_MILLISECONDS);
    }
}

}
