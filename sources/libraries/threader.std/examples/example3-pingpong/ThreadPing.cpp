#include "ThreadPing.h"

#include "Example3Common.h"

ThreadPing::ThreadPing()
    : ThreadBase(nullptr, "Thread.Ping")
    , _threadPong(nullptr)
{
}

void ThreadPing::setThreadPong(ThreadBase *threadPong)
{
    if (isRunning())
        return;

    _threadPong = threadPong;
}

void ThreadPing::onThreadStarted()
{
    writeLog(Message20);

    startThreadTimer(MESSAGE_NAME_SEND_PING, 1000, true);
}

void ThreadPing::onThreadFinished()
{
    writeLog(Message21);
}

void ThreadPing::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    const auto messageName = message->name();

    writeLog(Message22, messageName.c_str());

    if (MESSAGE_NAME_SEND_PING == messageName)
    {
        if (_threadPong)
        {
            _threadPong->postMessage(std::make_shared<MessageBase>(MESSAGE_NAME_PING));
            writeLog(Message23, MESSAGE_NAME_PING);
        }
    }
}
