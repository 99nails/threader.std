#include "ListThreads.h"

namespace Threader {

namespace Threads {

ThreadsStaticticCollector *ThreadsStaticticCollector::_instance = nullptr;

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
                                                    const bool terminated)
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

}}
