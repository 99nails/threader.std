#include "ThreadListenConnections.h"

#include "Example5Common.h"
#include "ThreadConnectionServer.h"

#include "SocketUtils.h"

ThreadListenConnections::ThreadListenConnections(MessageConsumer *parent)
    : ThreadListenSocketConnection(35976, parent)
{
}

void ThreadListenConnections::onThreadStarted()
{
    writeLog(Message100, portString().c_str());
}

void ThreadListenConnections::onThreadFinishing()
{
    writeLog(Message101, portString().c_str());
}

void ThreadListenConnections::onThreadFinished()
{
    writeLog(Message102, portString().c_str());
}

void ThreadListenConnections::onBeforeListenSocketInitialization(PollerListenSocketConnection *listener)
{
    writeLog(Message110, portString().c_str());
}

void ThreadListenConnections::onListenSocketInitialized(PollerListenSocketConnection *listener)
{
    writeLog(Message111, portString().c_str());
}

void ThreadListenConnections::onListenSocketError(PollerListenSocketConnection *sender,
                                                  int errorCode)
{
    writeLog(Message112, portString().c_str(), errorString(errorCode).c_str());
}

void ThreadListenConnections::onConnectionRequest(PollerListenSocketConnection *sender,
                                                  Descriptor socket,
                                                  const sockaddr_in &addr,
                                                  bool &accept)
{
    auto ipAddress = SocketUtils::sockAddrToString(addr);
    writeLog(Message120, sender->port(), ipAddress.c_str());

    accept = "127.0.0.1" == ipAddress;

    if (accept)
    {
        writeLog(Message121, sender->port(), ipAddress.c_str());
        if (parentThread())
        {
            auto thread = new ThreadConnectionServer(parentThread(),
                                                     ipAddress,
                                                     sender->port(),
                                                     socket);
            thread->start();
            auto message = std::make_shared<MessageThread>(MESSAGE_NAME_THREAD, thread);
            parentThread()->postMessage(message);
        }
    }
    else
        writeLog(Message122, sender->port(), ipAddress.c_str());
}
