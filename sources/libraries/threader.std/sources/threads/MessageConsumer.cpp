#include "MessageConsumer.h"

namespace Threader {

namespace Threads {

MessageConsumer::MessageConsumer()
{

}

void MessageConsumer::postMessage(const MessageBase::Ptr &message)
{
    _queue.enqueue(message);
}

void MessageConsumer::postMessages(const MessageList &messageList)
{
    _queue.enqueue(messageList);
}

void MessageConsumer::processMessages()
{
    const auto messages = _queue.dequeueAll();
    _messagesLeftToProcess = messages.size();
    if (0 == _messagesLeftToProcess)
        return;

    onProcessMessagesStarted(_messagesLeftToProcess);

    for (const auto &message : messages)
    {
        _messagesLeftToProcess--;
        processMessage(message);
    }

    onProcessMessagesFinished();
}

QueueMessages *MessageConsumer::queue()
{
    return &_queue;
}

} // namespace Threads
} // namespace Threader
