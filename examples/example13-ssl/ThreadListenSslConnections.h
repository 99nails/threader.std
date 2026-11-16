#pragma once

#include <ThreadListenSocketConnection.h>

using namespace Threader::Threads;

class ThreadListenSslConnections : public ThreadListenSocketConnection
{
public:
    explicit ThreadListenSslConnections(MessageConsumer *parent);

protected:
    void onThreadStarted();
    void onThreadFinishing();
    void onThreadFinished();

protected:
    void onBeforeListenSocketInitialization(PollerListenSocketConnection *listener);
    void onListenSocketInitialized(PollerListenSocketConnection *listener);

public:
    void onListenSocketError(PollerListenSocketConnection *sender, int errorCode);
    void onConnectionRequest(PollerListenSocketConnection *sender,
                             Descriptor socket,
                             const sockaddr_in &addr,
                             bool &accept);

};
