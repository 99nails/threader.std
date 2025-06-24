#pragma once

#include "ThreadBase.h"

// #include "../threader_global.h"

namespace Threader {

namespace Threads {

/**
 * @brief ThreadTimer - Класс потока таймера
 * Класс не используется вручную, а создается в методе ThreadBase.startTimer
 */
class ThreadTimer : public ThreadBase
{
    // Q_OBJECT
public:
    /**
     * @brief THREAD_NAME_PREFFIX - префика имени создаваемого потока
     */
    static const std::string THREAD_NAME_PREFFIX;

    /**
     * @brief ThreadTimer - Конструктор
     * @param parent - Поток-владелец
     * @param timerName - Имя сообщения таймера
     * @param delayMsecs - Задержка таймера в миллисекундах
     * @param multiShot - Признак множественного срабатывания таймера
     */
    explicit ThreadTimer(MessageConsumer *parent,
                         const std::string &timerName,
                         const uint32_t &delayMsecs,
                         const bool multiShot);

    /**
     * @brief timerName - Получение имени сообщения таймера
     * @return - Имя сообщения таймера
     */
    std::string timerName() const;

    /**
     * @brief delay - Получение задержки таймера в миллисекундах
     * @return - Задержка таймера в миллисекундах
     */
    int delay() const;

    /**
     * @brief multiShot  - Получение признака множественного срабатывания таймера
     * @return - Признак множественного срабатывания таймера
     */
    bool multiShot() const;

    /**
     * @brief asyncSetTimeout - Установка новой задержки таймера в миллисекундах
     * @param[in] delayMsecs - Задержка таймера в миллисекундах
     */
    void asyncSetTimeout(const int &delayMsecs);

    void processMessage(const MessageBase::Ptr &message) override;

protected:
    /**
     * @brief onIdle - Обработка ожидания потока
     */
    void onIdle() override;

    /**
     * @brief sendTimerMessage - Отправка сообщения таймера
     */
    virtual void sendTimerMessage();

private:
    /**
     * @brief _timerName - Имя сообщения таймера
     */
    std::string _timerName;

    /**
     * @brief _delay - Задержка таймера в миллисекундах
     */
    int _delay;

    /**
     * @brief _multiShot - Признак множественного срабатывания таймера
     */
    bool _multiShot;

    /**
     * @brief _shotCount - Счетчик срабатывания таймера
     */
    uint64_t _shotCount;
};

} // namespace Threads
} // namespace Threader
