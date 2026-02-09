#pragma once

#include "MemoryBlock.h"
#include "TrafficCounter.h"

#include "HandlerBase.h"
#include "PacketFactoryBase.h"
#include "ThreadBase.h"

#include <list>
#include <vector>

namespace Threader::Threads {

using namespace Threader::Utils;

class ConnectionDataConsumer;

class ConnectionBase : public HandlerDataConsumer
{
public:
    explicit ConnectionBase(ThreadBase *parentThread,
                            HandlerBase *handler,
                            const uint &reconnectTimeout = 0,
                            PacketFactoryBase *packetFactory = nullptr,
                            const bool &portionedIO = false);

    HandlerBase *handler() const;
    void setHandler(HandlerBase *handler);

    PacketFactoryBase *packetFactory() const;

    virtual void onBeforeWaitEvents();

    virtual void sendPacket(const PacketBase::Ptr &packet);

    virtual void sendData(const void* data, const size_t size);

    virtual void sendString(const std::string &data);

    uint64_t lastPacketSent() const;

    uint64_t lastPacketReceived() const;

    const TrafficCounter &inputTrafficCounter() const;

    const TrafficCounter &outputTrafficCounter() const;

    uint64_t connectedTickCount() const;

    const uint64_t &connectedDateTime() const;

    const uint64_t &disconnectedDateTime() const;

protected:

    virtual void onReadData(const char *data, const int &size);
    virtual void onWriteData(const char *data, const int &size);

private:
    void clearBuffers();

    HandlerBase *_handler;
    ThreadBase *_parentThread;
    ConnectionDataConsumer *_consumer;

    uint64_t _reconnectTimeout;
    uint64_t _nextTryToConnect;

    MemoryBlock::Ptr _inputBuffer;
    MemoryBlock::Ptr _outputBuffer;

    PacketFactoryBase *_packetFactory;

    uint64_t _lastPacketSent;
    uint64_t _lastPacketReceived;

    TrafficCounter _inputTrafficCounter;
    TrafficCounter _outputTrafficCounter;

    bool _portionedIO;

    uint64_t _connectedTickCount;

    uint64_t _connectedDateTime;
    uint64_t _disconnectedDateTime;

public:
    void onConnectionStateChanged(HandlerBase &sender,
                                  const HandlerBase::ConnectionState oldState,
                                  const HandlerBase::ConnectionState newState) override;

    void onReadyRead(HandlerBase &sender) override;
    void onReadyWrite(HandlerBase &sender) override;
    void onError(HandlerBase &sender,
                 const int errorCode,
                 const std::string &errorDescription) override;
};


using ConnectionVector = std::vector<ConnectionBase*>;
using ConnectionList = std::list<ConnectionBase*>;


class ConnectionDataConsumer
{
public:
    virtual void onConnectionStateChanged(const ConnectionBase &connection,
                                          const HandlerBase::ConnectionState oldState,
                                          const HandlerBase::ConnectionState newState) = 0;

    virtual void onError(const ConnectionBase &connection,
                         const int &errorCode,
                         const std::string &errorDescription) = 0;

    virtual void onReadData(const ConnectionBase &connection,
                            const char *data,
                            const int &size) = 0;

    virtual void onWriteData(const ConnectionBase &connection,
                             const char *data,
                             const int &size) = 0;

    virtual void onPacketReceived(const ConnectionBase &connection,
                                  const PacketBase::Ptr &packet) = 0;

    virtual void onPacketSent(const ConnectionBase &connection,
                              const PacketBase::Ptr &packet) = 0;
};

}
