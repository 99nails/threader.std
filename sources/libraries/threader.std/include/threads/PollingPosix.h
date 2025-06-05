
#pragma once

#ifdef linux

#include "ThreadsCommon.h"

#include <list>
#include <vector>
#include <poll.h>

#include <string>
#include <stdint.h>

namespace Threader {

namespace Threads {

/**
 * @brief The PollerBase class
 */
class PollerBase
{
public:
    /**
     * @brief PollerBase - Конструктор
     * @param descriptor - Дескриптор голосующего
     * @param events - Маска событий
     */
    explicit PollerBase(const int &descriptor = INVALID_DESCRIPTOR,
                        const short int events = 0);

    virtual ~PollerBase() = default;

    /**
     * @brief assign - Заполнение данными голосующего pollfd структуры,
     * предоставляемой снаружи для формирования массива ожидания
     * @param event - pollfd структура голосующего
     */
    virtual void assign(pollfd &event);

    /**
     * @brief process - Обработка события голосования
     * @param event - pollfd структура голосующего
     * @return - Признак обработки события
     */
    virtual bool process(const pollfd &event);

    /**
     * @brief descriptor - Получение дескриптора голосующего
     * @return  - Дескриптор голосующего
     */
    virtual int descriptor() const;

    /**
     * @brief setDescriptor - Установка дескриптора голосующего
     * @param descriptor - Дескритор голосующего
     */
    virtual void setDescriptor(const Descriptor &descriptor);

    /**
     * @brief events - Получение маски события голосующего
     * @return - Маска события
     */
    short int events();

    /**
     * @brief setEvents - Установка маски события голосующего
     * @param events - Маска события
     */
    void setEvents(short int events);

private:
    int _descriptor;
    short int _events;

// signals:
//     void signalOnPollEvent(const Threader::Threads::PollerBase *sender, const pollfd &event);
};

using  PollerList = std::list<PollerBase*>;

using PollerVector = std::vector<PollerBase*>;

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
     * @brief indexOfDescriptor - Получение индекса голосующего по его дескриптору
     * @param descriptor - Дескриптор
     * @return - Индекс голосующего
     */
    int indexOfDescriptor(const int &descriptor);

    /**
     * @brief _pollers - Список голосующих
     */
    PollerVector _pollers;

    /**
     * @brief _pollStructArray - Массив для хранения структур голосующих
     */
    std::vector<pollfd> _pollStructArray;

    /**
     * @brief _waitCount - Время в миллисекундах, проведеное в ожидании
     */
    uint64_t _waitCount;

    /**
     * @brief _threadName - имя потока-владельца
     */
    std::string _threadName;
};

}}

#endif
