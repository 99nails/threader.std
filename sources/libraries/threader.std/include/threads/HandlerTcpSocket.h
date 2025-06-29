#pragma once

#include "HandlerBase.h"

namespace Threader {

namespace Threads {

class HandlerTcpSocket : public HandlerBase
{
public:
    explicit HandlerTcpSocket(HandlerDataConsumer *consumer,
                              const std::string &host,
                              const uint16_t port,
                              const Descriptor descriptor = INVALID_DESCRIPTOR);
    ~HandlerTcpSocket() override;

    std::string host() const;

    uint16_t port() const;

    std::string ipAddress() const;

    int inputBytesAvailable() override;

    int outputBytesAvailable() override;

    bool reassignEndPoint(const std::string &host,
                          const int port);

    bool open() override;

    void close() override;

    TcpSocketType socketType() const;

#ifdef linux
    void assign(pollfd &event) override;

    bool process(const pollfd &event) override;
#endif

#ifdef WIN
protected:
    void registerEvent();
    void unregisterEvent();
#endif

#ifdef WIN
    int read(char *data, int count) override;

    int write(const char *data, int count) override;

    DescriptorsVector *events() override;

    bool process(Descriptor eventToProcess) override;
#endif

private:
    std::string _host;
    uint16_t _port;
    std::string _ipAddress;
    TcpSocketType _socketType;
};


}}
