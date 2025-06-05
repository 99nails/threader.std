#include "ThreadPong.h"

#include "Example3Common.h"

ThreadPong::ThreadPong()
    : ThreadBase(nullptr, "Thread.Ping")
    , _threadPing(nullptr)
{
}

void ThreadPong::setThreadPing(ThreadBase *threadPing)
{
    _threadPing = threadPing;
}

void ThreadPong::onThreadStarted()
{
    writeLog(Message30);
}

void ThreadPong::onThreadFinished()
{
    writeLog(Message31);
}

void ThreadPong::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    const auto messageName = message->name();

   writeLog(Message32, messageName.c_str());

    if (_threadPing)
    {
        _threadPing->postMessage(std::make_shared<MessageBase>(MESSAGE_NAME_PONG));
        writeLog(Message33, MESSAGE_NAME_PONG);
    }
}
