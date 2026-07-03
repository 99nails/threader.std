#include "ThreadPing.h"

#include "Example3Common.h"

ThreadPing::ThreadPing()
    : ThreadBase(nullptr, "Thread.Ping")
    , _threadPong(nullptr)
{
}

void ThreadPing::setThreadPong(ThreadBase *threadPong)
{
    if (isThreadRunning())
        return;

    _threadPong = threadPong;
}

void ThreadPing::onThreadStarted()
{
    writeLog(Message120);

    startThreadTimer(MESSAGE_NAME_SEND_PING, 1000, true);
}

void ThreadPing::onThreadFinished()
{
    writeLog(Message121);
}

void ThreadPing::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    const auto messageName = message->name();

    writeLog(Message122, messageName.c_str());

    if (MESSAGE_NAME_SEND_PING == messageName)
    {
        if (_threadPong)
        {
            _threadPong->postMessage1<MessageBase>(MESSAGE_NAME_PING);
            writeLog(Message123, MESSAGE_NAME_PING);
        }
    }
}
