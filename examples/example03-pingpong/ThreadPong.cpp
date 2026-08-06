#include "ThreadPong.h"

#include "Example03Common.h"

ThreadPong::ThreadPong()
    : ThreadBase(nullptr, "Thread.Pong")
    , _threadPing(nullptr)
{
}

void ThreadPong::setThreadPing(ThreadBase *threadPing)
{
    _threadPing = threadPing;
}

void ThreadPong::onThreadStarted()
{
    writeLog(Message130);
}

void ThreadPong::onThreadFinished()
{
    writeLog(Message131);
}

void ThreadPong::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    const auto messageName = message->name();

   writeLog(Message132, messageName.c_str());

    if (_threadPing)
    {
        _threadPing->postMessage1<MessageBase>(MESSAGE_NAME_PONG);
        writeLog(Message133, MESSAGE_NAME_PONG);
    }
}
