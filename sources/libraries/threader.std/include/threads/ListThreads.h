#pragma once

#include "ThreadBase.h"
// #include "../threader_global.h"

// #include <QList>
// #include <QHash>
// #include <QMutex>

#include <list>
#include <map>
#include <mutex>

namespace Threader {

namespace Threads {


typedef struct ThreadStatistic
{
    long ThreadId;
    std::string ThreadName;
    uint64_t StartedMsecsSinceEpoch;
    uint64_t AliveMsecsSinceEpoch;
    uint64_t AliveTickCount;
    uint64_t WaitMSecsCount;
    int QueueCount;
    bool Terminated;
} ThreadStatisticStruct;

using ListStatistic = std::list<ThreadStatisticStruct>;

using HashThreadsStatistic = std::map<ThreadBase*, ThreadStatisticStruct>;

class ThreadsStaticticCollector
{
public:
    explicit ThreadsStaticticCollector();
    virtual ~ThreadsStaticticCollector();

    static ThreadsStaticticCollector *instance();

    /**
     * @brief registerThread - Регистрация потока в общем списке потоков
     * @param thread - Регистрирующийся поток
     */
    void registerThread(ThreadBase *thread);

    /**
     * @brief unregisterThread - Отмена регистрации потока в списке потоков
     * @param thread - Удаляемый поток
     */
    void unregisterThread(ThreadBase *thread);

    /**
     * @brief threadByClassName - Получение экземпляра потока по имени класса потока
     * @param threadClassName - Имя класса потока
     * @return - Экземпляр класса потока
     */
    ThreadBase* threadByClassName(const std::string &threadClassName);

    /**
     * @brief accumulateStatistic - Хранение статистики в памяти
     * @param thread - Указатель на поток
     * @param startedMsecsSinceEpoch - Время старта
     * @param aliveMsecsSinceEpoch - Время снятия статистики
     * @param waitMSecsCount - Ожидание в миллисекундах
     * @return - Статистика добавлена
     */
    bool accumulateStatistic(ThreadBase *thread,
                             const long threadId,
                             const std::string &threadName,
                             const uint64_t &startedMsecsSinceEpoch,
                             const uint64_t &aliveMsecsSinceEpoch,
                             const uint64_t &aliveTickCount,
                             const uint64_t &waitMSecsCount,
                             const int &queueCount,
                             const bool terminated);

    /**
     * @brief statistic - Получение списка со статистикой потоков
     * @return - Список со статистикой потоков
     */
    ListStatistic statistic();

private:

    static ThreadsStaticticCollector *_instance;

    VectorThreadBase _threads;

    HashThreadsStatistic _statistic;

    std::mutex *_mutex;
};

}}
