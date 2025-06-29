#pragma once

#include "ThreadsCommon.h"

#ifdef linux
#include "PollingPosix.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

#ifdef WIN
#include "PollingsWindows.h"
#include <windows.h>
#endif

#include <vector>

namespace Threader::Threads
{

class ConsumerListenSocketEvents;

class PollerListenSocket : public PollerBase
{
public:
    explicit PollerListenSocket(ConsumerListenSocketEvents *consumer,
                                uint16_t port);
    ~PollerListenSocket() override;

    bool isInitialized() const;

    uint16_t port() const;

    bool initialize();
    void finalize();

#ifdef WIN
    Descriptor descriptor() const;
    void setDescriptor(const Descriptor descriptor);
#endif

#ifdef linux
    bool process(const pollfd &event) override;
    int processConnectionRequests();
#endif

private:
    bool _isInitialized;
    uint16_t _port;
    ConsumerListenSocketEvents *_consumer;

#ifdef WIN
    Descriptor _descriptor;
    Descriptor _event;

    void finalizeWindows();
    bool process(Descriptor eventToProcess) override;
#endif

#ifdef linux
    void finalizeLinux();
#endif
};


using PollerListenSocketVector = std::vector<PollerListenSocket*>;


class ConsumerListenSocketEvents
{
public:

    virtual void onListenSocketError(PollerListenSocket *sender,
                                     int errorCode) = 0;

    virtual void onConnectionRequest(PollerListenSocket *sender,
                                     Descriptor socket,
                                     const sockaddr_in &addr,
                                     bool &accept) = 0;
};

}
