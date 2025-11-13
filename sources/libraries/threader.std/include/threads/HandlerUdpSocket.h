#pragma once

#include "HandlerBase.h"

#ifdef linux
#include <netinet/in.h>
#endif

namespace Threader::Threads {

class HandlerUdpSocket : public HandlerBase
{
public:
    explicit HandlerUdpSocket(HandlerDataConsumer *consumer,
                              const std::string &host,
                              const uint16_t port);

    std::string host() const;

    uint16_t port() const;

    std::string ipAddress() const;

    UdpSocketType socketType() const;

    bool open() override;

    void close() override;

    int inputBytesAvailable() override;

    int outputBytesAvailable() override;

#ifdef linux
    void assign(pollfd &event) override;

    bool process(const pollfd &event) override;
#endif

#ifdef WIN32
    DescriptorsVector *events() override;

    bool process(Descriptor eventToProcess) override;
#endif

    int read(char *data, int count) override;

    int write(const void *data, int count) override;

    sockaddr_in remoteAddress() const;

private:
    std::string _host;
    uint16_t _port;
    std::string _ipAddress;
    UdpSocketType _socketType;
    sockaddr_in _localAddress;
    sockaddr_in _remoteAddress;

#ifdef WIN32
    void registerEvent();
    void unregisterEvent();
#endif
};

}
