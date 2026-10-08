#pragma once

#include "HandlerBase.h"

#ifdef linux
#include <netinet/in.h>
#endif

#include <map>

namespace Threader::Threads {

constexpr char INADDR_ANY_IP[] = "0.0.0.0";

class HandlerUdpSocket : public HandlerBase
{
public:
    explicit HandlerUdpSocket(HandlerDataConsumer *consumer,
                              const std::string &host,
                              const uint16_t port,
                              const UdpSocketType udpSocketType,
                              const std::string &bindIpAddress = INADDR_ANY_IP);

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

    int read(void *data, int count) override;

    int write(const void *data, int count) override;

    int writeTo(const sockaddr_in &address,
                const void *data,
                int count);

    int writeTo(const std::string &ipAddress,
                const void *data,
                int count);

    const sockaddr_in remoteAddress() const;

    const sockaddr_in &readAddress() const;

private:
    std::string _host;
    uint16_t _port;
    std::string _ipAddress;
    UdpSocketType _socketType;
    sockaddr_in _bindAddress;
    sockaddr_in _remoteAddress;
    sockaddr_in _readAddress;
    std::map<std::string, sockaddr_in> _addressesMap;

#ifdef WIN32
    void registerEvent();
    void unregisterEvent();
#endif
};

}
