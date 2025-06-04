#include "QueueMessages.h"

namespace Threader {

namespace Threads {

QueueMessages::QueueMessages()
{
    _mutex = new std::mutex();
}

QueueMessages::~QueueMessages()
{
    delete _mutex;
}

int QueueMessages::count()
{
    std::lock_guard<std::mutex> lock(*_mutex);

    return _queue.size();
}

int QueueMessages::enqueue(const MessageBase::Ptr& message)
{
    std::lock_guard<std::mutex> lock(*_mutex);

    _queue.push(message);
    return _queue.size();
}

int QueueMessages::enqueue(const MessageList &list)
{
    std::lock_guard<std::mutex> lock(*_mutex);

    for (const auto &message : list)
        _queue.push(message);

    return _queue.size();
}

MessageBase::Ptr QueueMessages::dequeue()
{
    std::lock_guard<std::mutex> lock(*_mutex);

    if (!_queue.empty())
    {
        auto &message = _queue.front();
        _queue.pop();
        return message;
    }

    return MessageBase::Ptr();
}

MessageList QueueMessages::dequeueAll()
{
    std::lock_guard<std::mutex> lock(*_mutex);

    int size = _queue.size();
    MessageList result;

    for (int i = 0; i < size; i++)
    {
        result.push_back(_queue.front());
        _queue.pop();
    }

    return result;
}

}}
