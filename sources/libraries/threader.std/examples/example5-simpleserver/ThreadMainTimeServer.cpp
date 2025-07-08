#include "ThreadMainTimeServer.h"

#include "Example5Common.h"
#include "ThreadListenConnections.h"

#include "MessageBase.h"

constexpr char APPLICATION_NAME[] = "Time.Server";

ThreadMainTimeServer::ThreadMainTimeServer()
    : ThreadBase(nullptr, "Thread.Main")
{
}


void ThreadMainTimeServer::onThreadStarted()
{
    writeLog(Message10, APPLICATION_NAME);
}

void ThreadMainTimeServer::onThreadFinishing()
{
    writeLog(Message11, APPLICATION_NAME);
}

void ThreadMainTimeServer::onThreadFinished()
{
    writeLog(Message12, APPLICATION_NAME);
}

void ThreadMainTimeServer::startChildThreads()
{
    _threadListen = registerAndStartChildThread(new ThreadListenConnections(this));
}


void ThreadMainTimeServer::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    auto messageName = message->name();
    if (MESSAGE_NAME_THREAD == messageName)
        doMessageThread(std::dynamic_pointer_cast<MessageThread>(message));
}

void ThreadMainTimeServer::doMessageThread(const MessageThread::Ptr &message)
{
    if (!message)
        return;

    registerAndStartChildThread(message->value(), false);
}
