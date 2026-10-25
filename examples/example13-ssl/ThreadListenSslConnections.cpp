#include "ThreadListenSslConnections.h"

#include "Example13Common.h"
#include "ThreadSslServerConnection.h"

#include <SocketUtils.h>

ThreadListenSslConnections::ThreadListenSslConnections(MessageConsumer *parent)
    : ThreadListenSocketConnection({PORT_TCP, PORT_SSL}, parent)
{
}

void ThreadListenSslConnections::onThreadStarted()
{
    writeLog(Message100, portString().c_str());
}

void ThreadListenSslConnections::onThreadFinishing()
{
    writeLog(Message101, portString().c_str());
}

void ThreadListenSslConnections::onThreadFinished()
{
    writeLog(Message102, portString().c_str());
}

void ThreadListenSslConnections::onBeforeListenSocketInitialization(PollerListenSocketConnection *listener)
{
    writeLog(Message110, std::to_string(listener->port()).c_str());
}

void ThreadListenSslConnections::onListenSocketInitialized(PollerListenSocketConnection *listener)
{
    writeLog(Message111, std::to_string(listener->port()).c_str());
}

void ThreadListenSslConnections::onListenSocketError(PollerListenSocketConnection *sender,
                                                     int errorCode)
{
    writeLog(Message112, std::to_string(sender->port()).c_str(), errorString(errorCode).c_str());
}

void ThreadListenSslConnections::onConnectionRequest(PollerListenSocketConnection *sender,
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

        bool useSsl = sender->port() == PORT_SSL;

        if (parentThread())
        {
            auto thread = new ThreadSslServerConnection(parentThread(),
                                                        ipAddress,
                                                        sender->port(),
                                                        socket,
                                                        useSsl);
            thread->startThread();
            parentThread()->postMessage1<MessageThread>(thread);
        }
    }
    else
        writeLog(Message122, sender->port(), ipAddress.c_str());
}
