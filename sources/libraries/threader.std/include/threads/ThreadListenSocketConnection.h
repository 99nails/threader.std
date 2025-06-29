#pragma once

#include "MessageConsumer.h"
#include "PollerListenSocketConnection.h"
#include "ThreadBase.h"

#include <vector>

namespace Threader::Threads {

using PortVector = std::vector<uint16_t>;

class ThreadListenSocketConnection : public ThreadBase, ConsumerListenSocketEvents
{
public:
    explicit ThreadListenSocketConnection(uint16_t port,
                                          MessageConsumer *parent = nullptr);
    explicit ThreadListenSocketConnection(const PortVector &ports,
                                          MessageConsumer *parent = nullptr);
    ~ThreadListenSocketConnection() override;

    uint16_t port(int index) const;

    PollerListenSocketConnection *listeners(int index) const;

protected:
    void registerListener(PollerListenSocketConnection *listener);
    std::string portString();

    void onBeforeWaitEvents() override;
    void terminateChildThreads() override;

    virtual void onBeforeListenSocketInitialization(PollerListenSocketConnection *listener) = 0;

    virtual void onListenSocketInitialized(PollerListenSocketConnection *listener) = 0;

private:
    PortVector _ports;
    std::string _portsString;
    uint64_t _nextStartListening;

    PollerListenSocketVector _listeners;

    static const uint TIMEOUT_REOPEN_LISTEN_SOCKET_MILLISECONDS;

protected:
    void slotOnListenSocketError(PollerListenSocketConnection *sender,
                                 int errorCode);

    void slotOnConnectionRequest(PollerListenSocketConnection *sender,
                                 Descriptor socket,
                                 const sockaddr_in &addr,
                                 bool &accept);

};

}
