#include "ThreadMainSslServer.h"

#include "ThreadListenSslConnections.h"
#include "ThreadSslServerConnection.h"

#include "SignalHandler.h"

ThreadMainSslServer::ThreadMainSslServer(const std::string &applicationBinaryName,
                                         const std::string &applicationFriendlyName)
    : ThreadMainWithoutSettings(nullptr,
                               applicationBinaryName,
                               applicationFriendlyName)
    , _threadListen(nullptr)
{

}

void ThreadMainSslServer::startChildThreads()
{
    _threadListen = registerAndStartChildThread(new ThreadListenSslConnections(this));
}


void ThreadMainSslServer::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    const auto messageName = message->name();

    if (MessageThread::MESSAGE_NAME == messageName)
        doMessageThread(std::dynamic_pointer_cast<MessageThread>(message));
    else if (MessagePacket::MESSAGE_NAME == messageName)
        doMessagePacket(std::dynamic_pointer_cast<MessagePacket>(message));
}

void ThreadMainSslServer::doMessageThread(const MessageThread::Ptr &message)
{
    if (!message || !message->value())
        return;

    registerAndStartChildThread(message->value(), false);
}

void ThreadMainSslServer::doMessagePacket(const MessagePacket::Ptr &message)
{
    if (!message)
        return;

    auto sslThreadfName = demangle(typeid(ThreadSslServerConnection).name());

    for (int i = 0; i < childThreadCount(); i++)
    {
        auto thread = childThread(i);
        if (thread->thisThreadId() == message->threadId() || thread->threadClassName() != sslThreadfName)
            continue;

        thread->postMessage(message);
    }
}
