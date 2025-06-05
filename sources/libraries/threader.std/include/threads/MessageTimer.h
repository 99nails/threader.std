#pragma once

#include "MessageBase.h"

namespace Threader {

namespace Threads {

/**
 * @brief MessageTimer - Класс сообщения таймера
 */
class MessageTimer : public MessageBase {
  public:
    using Ptr = std::shared_ptr<MessageTimer>;

  public:
    /**
     * @brief MessageTimer - Конструктор
     * @param name - Имя сообщения таймера
     * @param shotCount - Счетчик сообщения
     */
      explicit MessageTimer(const std::string &name, const uint64_t &shotCount);

    /**
     * @brief shotCount - Получение счетчика сообщения
     * @return  - Счетчик сообщения
     */
    uint64_t shotCount() const;

  private:
    /**
     * @brief _shotCount - Счетчик сообщения
     */
    int64_t _shotCount;
};

/**
 * @brief MessageTimer - Класс сообщения таймеру о необходимости изменения задержки срабатывания
 */
class MessageTimerSetTimeout : public MessageBase {
  public:
    using Ptr = std::shared_ptr<MessageTimerSetTimeout>;

  public:
    /**
     * @brief MessageTimer - Конструктор
     * @param name - Имя сообщения таймера
     * @param shotCount - Счетчик сообщения
     */
      explicit MessageTimerSetTimeout(const std::string &name, const int &newDelay);

    /**
     * @brief shotCount - Получение ного значения задержки
     */
    int delay() const { return _delay; }

  private:
    /**
     * @brief _delay - Задержка
     */
    int _delay;
};

} // namespace Threads
} // namespace Threader
