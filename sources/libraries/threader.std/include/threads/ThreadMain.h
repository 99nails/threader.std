#pragma once

#include "LogMessagesTemplates.h"
#include "ListThreads.h"
#include "MessageWriteToFile.h"
#include "SignalHandler.h"
#include "StdStringUtils.h"
#include "ThreadBase.h"

#ifdef linux
#include <sys/resource.h>
#endif

namespace Threader::Threads
{

constexpr uint TIMEOUT_BUILD_STATISTIC_MILLISECONDS = 10000;
constexpr uint TIMEOUT_BUILD_COMMON_STATISTIC_MILLISECONDS = 60000;

template <class SettingsSet>
class ThreadMain : public ThreadBase
{
public:
    explicit ThreadMain(MessageConsumer *parent = nullptr,
                        const std::string &applicationBinaryName = std::string(),
                        const std::string &applicationFriendlyName = std::string(),
                        const std::string &threadName = std::string(),
                        const std::string &iniFileName = std::string())
        : ThreadBase(parent,
                     (threadName.empty()) ? "Thread.Main" : threadName)
        , _applicationBinaryName(applicationBinaryName)
        , _applicationFriendlyName(applicationFriendlyName)
        , _nextBuildStatisticTickCount(
              DateTimeUtils::getNextTickCount(TIMEOUT_BUILD_STATISTIC_MILLISECONDS))
        , _nextBuildCommonStatisticTickCount(0)
        , _threadStatisticFileName(applicationBinaryName + ".Threads")
    {
        _settings.setFileName(iniFileName);
        if (_applicationFriendlyName.empty())
            _applicationFriendlyName = threadName;
    }

    SettingsSet &settings()
    {
        return _settings;
    }
protected:
    void onThreadStarted() override
    {
        writeLog(Message10, _applicationFriendlyName.c_str());

        writeLog(Message20, _applicationFriendlyName.c_str(), _settings.fileName().c_str());
        if (_settings.read())
            writeLog(Message21, _applicationFriendlyName.c_str(), _settings.fileName().c_str());
        else
            writeLog(Message22, _applicationFriendlyName.c_str(), _settings.fileName().c_str());
    }

    void onThreadFinishing() override
    {
        writeLog(Message11, _applicationFriendlyName.c_str());
    }

    void onThreadFinished() override
    {
        writeLog(Message12, _applicationFriendlyName.c_str());
    }

    void onIdle() override
    {
        snapshotThreads();
    }

private:
    void snapshotThreads(bool rightNow = false)
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
            logThreadInstance->postMessage(std::make_shared<MessageWriteToFile>(
                                               _threadStatisticFileName,
                                               statisticString,
                                               MessageWriteToFile::WriteMode::Truncate));
        }

        if (rightNow || _nextBuildCommonStatisticTickCount <= tickCount)
        {
            writeLog(Message14, i, memoryUsage);

            _nextBuildCommonStatisticTickCount = DateTimeUtils::getNextTickCount(TIMEOUT_BUILD_COMMON_STATISTIC_MILLISECONDS);
        }
    }


private:
    std::string _iniFileName;
    SettingsSet _settings;
    std::string _applicationBinaryName;
    std::string _applicationFriendlyName;
    uint64_t _nextBuildStatisticTickCount;
    uint64_t _nextBuildCommonStatisticTickCount;
    std::string _threadStatisticFileName;

};

}
