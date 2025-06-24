#pragma once

#ifdef _WIN32

#include "ThreadsCommon.h"

#include <list>
#include <vector>
#include <string>
#include <windows.h>

namespace Threader::Threads {

using DescriptorsVector = std::vector<Descriptor>;

class PollerBase
{
public:
    explicit PollerBase(const DescriptorsVector &events = {});
    virtual ~PollerBase();

    bool hasEvent(Descriptor event);
    virtual DescriptorsVector *events();
    virtual void assign(const DescriptorsVector &events);
    virtual bool process(Descriptor eventToProcess);

private:
    DescriptorsVector _events;
};

using PollersList = std::list<PollerBase*>;

/**
 * @brief Polling - Класс-обертка функционала polling-а
 */
class Polling
{
public:
    /**
     * @brief Polling - Конструктор
     */
    explicit Polling(const std::string &threadName = std::string());

    /**
     * @brief pollersCount - Получение количества зарегистрированных голосующих
     * @return - Количество зарегистрированных голосующих
     */
    int pollersCount();

    /**
     * @brief registerPoller - Регистрация голосующего
     * @param poller - Голосующий
     * @return - Количество зарегистрированных голосующих
     */
    int registerPoller(PollerBase *poller);

    /**
     * @brief unregisterPoller - Снятие регистрации голосующего
     * @param poller - Голосующий
     * @return - Количество зарегистрированных голосующих
     */
    int unregisterPoller(PollerBase *poller);

    /**
     * @brief poll - Выполнение голосования
     * @param timeout - Время ожидания события голосования в милисекундах
     * @return
     */
    int poll(const uint32_t &timeout);

    /**
     * @brief waitCount - Получение времени режиме ожидания голосования в миллисекундах
     * @return - Время в режиме ожидания голосования в миллисекундах
     */
    int64_t waitCount() const;

    /**
     * @brief threadName - Получение имени потока-владельца
     * @return
     */
    const std::string &threadName() const;

    /**
     * @brief setThreadName - Установка имени потока-владельца
     * @param newThreadName
     */
    void setThreadName(const std::string &threadName);

private:
    /**
     * @brief _pollers - Список голосующих
     */
    PollersList _pollers;

    /**
     * @brief _waitCount - Время в миллисекундах, проведеное в ожидании
     */
    int64_t _waitCount;

    /**
     * @brief _threadName - имя потока-владельца
     */
    std::string _threadName;
};

}

#endif
