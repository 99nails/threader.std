#pragma once

#include "ThreadListenSocketConnection.h"
#include "SettingsTimeServer.h"

using namespace Threader::Threads;

class ThreadListenConnections : public ThreadListenSocketConnection
{
public:
    explicit ThreadListenConnections(MessageConsumer *parent,
                                     const SettingsTimeServer::Ptr &settings);
    virtual ~ThreadListenConnections() {};

    // ThreadBase interface

protected:
    void onThreadStarted();
    void onThreadFinishing();
    void onThreadFinished();

    // ThreadListenSocketConnection interface
protected:
    void onBeforeListenSocketInitialization(PollerListenSocketConnection *listener);
    void onListenSocketInitialized(PollerListenSocketConnection *listener);

    // ConsumerListenSocketEvents interface
public:
    void onListenSocketError(PollerListenSocketConnection *sender, int errorCode);
    void onConnectionRequest(PollerListenSocketConnection *sender,
                             Descriptor socket,
                             const sockaddr_in &addr,
                             bool &accept);

private:
    SettingsTimeServer::Ptr _settings;
};

