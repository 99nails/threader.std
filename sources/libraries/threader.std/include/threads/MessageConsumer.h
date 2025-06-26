#pragma once

#include "MessageBase.h"
#include "QueueMessages.h"

namespace Threader {

namespace Threads {

class MessageConsumer
{
public:
    explicit MessageConsumer();

    virtual ~MessageConsumer() = default;

    /**
     * @brief postMessage - Оправка сообщения потоку
     * @param message - Сообщения для потока
     */
    virtual void postMessage(const MessageBase::Ptr &message);

    /**
     * @brief postMessages - Отправка потоку группы сообщений
     * @param messageList - Группа сообщений
     */
    virtual void postMessages(const MessageList &messageList);

    /**
     * @brief postTerminateEvent - Отправка события завершения потока
     */
    virtual void postTerminateEvent() = 0;

    /**
     * @brief postEventChildThreadTerminated - отправка события завершения потоку владельцу
     */
    virtual void postEventChildThreadTerminated() = 0;

protected:
    /**
     * @brief messagesLeftToProcess - получение остатка сообщений для пакетной обработки
     * @return
     */
    int messagesLeftToProcess();

    /**
     * @brief onProcessMessagesStarted - событие начала шага цикла обработки сообщений
     */
    virtual void onProcessMessagesStarted(const int) {}

    /**
     * @brief onProcessMessagesFinished - событие завершения шага цикла обработки сообщений
     */
    virtual void onProcessMessagesFinished() {}

    /**
     * @brief processMessage - Виртуальный метод вызываемый при обработке сообщений для потока.
     * Предназначен для перекрытия в дочерних потоках
     */
    virtual void processMessage(const MessageBase::Ptr &) = 0;

    /**
     * @brief processMessages - Обработка входящих сообщений из очереди
     */
    virtual void processMessages();

    QueueMessages *queue();

private:
    /**
     * @brief _queue - Очередь входящих сообщений потока
     */
    QueueMessages _queue;

    /**
     * @brief _messagesLeftToProcess - остаток сообщений для пакетной обработки
     */
    int _messagesLeftToProcess;
};

using MessageConsumerVector = std::vector<MessageConsumer>;

} // namespace Threads
} // namespace Threader
