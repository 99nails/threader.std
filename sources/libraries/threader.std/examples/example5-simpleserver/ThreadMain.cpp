#include "ThreadMain.h"

#include "Example5Common.h"
#include "ThreadListenConnections.h"

#include "MessageBase.h"

ThreadMainExample5::ThreadMainExample5()
    : ThreadBase(nullptr, "Thread.Main")
{
}


void ThreadMainExample5::onThreadStarted()
{
    writeLog(Message10);
}

void ThreadMainExample5::onThreadFinishing()
{
    writeLog(Message11);
}

void ThreadMainExample5::onThreadFinished()
{
    writeLog(Message12);
}

void ThreadMainExample5::startChildThreads()
{
    _threadListen = registerAndStartChildThread(new ThreadListenConnections(this));
}


void ThreadMainExample5::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    auto messageName = message->name();
    if (MESSAGE_NAME_THREAD == messageName)
        doMessageThread(std::dynamic_pointer_cast<MessageThread>(message));
}

void ThreadMainExample5::doMessageThread(const MessageThread::Ptr &message)
{
    if (!message)
        return;

    registerAndStartChildThread(message->value(), false);
}
