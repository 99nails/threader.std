#include "ListThreads.h"

#include "LogMessagesTemplates.h"
#include "MessageWriteToFile.h"
#include "SignalHandler.h"
#include "StdStringUtils.h"

#ifdef WIN32
#include <psapi.h>
#endif

#ifdef linux
#include <sys/resource.h>
#endif


namespace Threader {

namespace Threads {

using namespace Threader::Utils;

ThreadsStaticticCollector *ThreadsStaticticCollector::_instance = nullptr;

std::string ThreadsStaticticCollector::_threadStatisticFileName = "";

ThreadsStaticticCollector::ThreadsStaticticCollector()
{
    _mutex = new std::mutex;
}

ThreadsStaticticCollector::~ThreadsStaticticCollector()
{
    delete _mutex;
}

ThreadsStaticticCollector *ThreadsStaticticCollector::instance()
{
    if (!_instance)
        _instance = new ThreadsStaticticCollector();
    return _instance;
}

void ThreadsStaticticCollector::registerThread(ThreadBase *thread)
{
    std::lock_guard<std::mutex> lock(*_mutex);
    _threads.push_back(thread);
}

void ThreadsStaticticCollector::unregisterThread(ThreadBase *thread)
{
    std::lock_guard<std::mutex> lock(*_mutex);
    // _threads.removeAll(thread);

    auto iterator = _threads.begin();
    while (iterator != _threads.end())
    {
        auto currentThread = (*iterator);
        if (currentThread == thread)
        {
            iterator = _threads.erase(iterator);
        }
        else
        {
            iterator++;
        }
    }
    _statistic.erase(thread);
}

ThreadBase *ThreadsStaticticCollector::threadByClassName(const std::string &threadClassName)
{
//    ThreadBase *thread;

    std::lock_guard<std::mutex> lock(*_mutex);

    for (size_t i = 0; i < _threads.size(); i++)
    {
        auto thread = _threads.at(i);
        auto name = typeid(*thread).name();
        if (name == threadClassName)
            return thread;
    }
    return nullptr;
}

bool ThreadsStaticticCollector::accumulateStatistic(ThreadBase *thread,
                                                    const THREAD_ID threadId,
                                                    const std::string &threadName,
                                                    const uint64_t &startedMsecsSinceEpoch,
                                                    const uint64_t &aliveMsecsSinceEpoch,
                                                    const uint64_t &aliveTickCount,
                                                    const uint64_t &waitMSecsCount,
                                                    const int &queueCount,
                                                    const bool terminated,
                                                    const bool finished)
{
    if (!thread)
        return false;

    std::lock_guard<std::mutex> lock(*_mutex);

    ThreadStatisticStruct stat;
    stat.ThreadId = threadId;
    stat.ThreadName = threadName;
    stat.StartedMsecsSinceEpoch = startedMsecsSinceEpoch;
    stat.AliveMsecsSinceEpoch = aliveMsecsSinceEpoch;
    stat.AliveTickCount = aliveTickCount;
    stat.WaitMSecsCount = waitMSecsCount;
    stat.QueueCount = queueCount;
    stat.Terminated = terminated;
    stat.Finished = finished;
    _statistic[thread] = stat;
    return true;
}

ListStatistic ThreadsStaticticCollector::statistic()
{
    std::lock_guard<std::mutex> lock(*_mutex);

    ListStatistic result;
    for (size_t i = 0; i < _threads.size(); i++)
    {
        ThreadBase *thread = _threads.at(i);

        auto iterator = _statistic.find(thread);
        if (iterator != _statistic.end())
            result.push_back(_statistic[thread]);
    }
    return result;
}

void ThreadsStaticticCollector::sendSnapshot()
{
    if (!ThreadBase::logThread())
        return;

    auto statisticList = statistic();

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
    for (const auto &statistic: statisticList)
    {
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
                ((statistic.Terminated) ? " Terminated" : "") +
                ((statistic.Finished) ? " Finished" : "") +
                "\r\n";

    }

#ifdef linux
    setThreadInfo(statisticString);
#endif

    statisticString += "{EOF}" + std::string(DateTimeUtils::msecsSinceEpochLocal() % 8, ' ');

    ThreadBase *logThreadInstance = ThreadBase::logThread();
    if (logThreadInstance)
    {
        logThreadInstance->postMessage1<MessageWriteToFile>(_threadStatisticFileName,
                                                            statisticString,
                                                            MessageWriteToFile::WriteMode::Truncate);
    }

    ThreadBase::writeLog(Message15, i, memoryUsage);
}

void ThreadsStaticticCollector::setThreadStatisticFileName(const std::string &threadStatisticFileName)
{
    _threadStatisticFileName = threadStatisticFileName;
}

}}
