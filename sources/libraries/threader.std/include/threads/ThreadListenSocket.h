#pragma once

#include "MessageConsumer.h"
#include "PollerListenSocket.h"
#include "ThreadBase.h"

#include <vector>

namespace Threader::Threads {

using PortVector = std::vector<uint16_t>;

class ThreadListenSocket : public ThreadBase, ConsumerListenSocketEvents
{
public:
    explicit ThreadListenSocket(uint16_t port,
                                MessageConsumer *parent = nullptr);
    explicit ThreadListenSocket(const PortVector &ports,
                                MessageConsumer *parent = nullptr);
    ~ThreadListenSocket() override;
    uint16_t port(int index) const;

    PollerListenSocket *listeners(int index) const;

protected:
    void registerListener(PollerListenSocket *listener);
    std::string portString();

    void onBeforeWaitEvents() override;
    void terminateChildThreads() override;

    virtual void onBeforeListenSocketInitialization(PollerListenSocket *listener) = 0;

    virtual void onListenSocketInitialized(PollerListenSocket *listener) = 0;

//    virtual void onAcceptConnectionRequest(PollerListenSocket *listener,
//                                           Descriptor socket,
//                                           const std::string &ipAddress,
//                                           bool &accept) = 0;

//    void onListenSocketError(PollerListenSocket *listener,
//                             int errorCode) override;

private:
    PortVector _ports;
    std::string _portsString;
    uint64_t _nextStartListening;

    PollerListenSocketVector _listeners;

    static const uint TIMEOUT_REOPEN_LISTEN_SOCKET_MILLISECONDS;

protected:
    void slotOnListenSocketError(PollerListenSocket *sender,
                                   int errorCode);

    void slotOnConnectionRequest(PollerListenSocket *sender,
                                 Descriptor socket,
                                 const sockaddr_in &addr,
                                 bool &accept);

};

}
