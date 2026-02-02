#pragma once

#include "HandlerBase.h"
#include "PacketFactoryBase.h"
#include "ThreadBase.h"

#include "TrafficCounter.h"


namespace Threader::Threads {


using namespace Threader::Utils;


class ThreadHandler : public ThreadBase,
                      public HandlerDataConsumer
{
    // Q_OBJECT
public:
    static const std::string MESSAGE_NAME_DEVICE_DATA_INPUT;
    static const std::string MESSAGE_NAME_DEVICE_DATA_OUTPUT;
    static const std::string MESSAGE_NAME_DEVICE_CONNECTING;
    static const std::string MESSAGE_NAME_DEVICE_CONNECTED;
    static const std::string MESSAGE_NAME_DEVICE_DISCONNECTED;
    static const std::string MESSAGE_NAME_DEVICE_ERROR;

    explicit ThreadHandler(MessageConsumer *parent,
                           HandlerBase *handler,
                           const uint32_t &reconnectTimeout = 0,
                           PacketFactoryBase *packetFactory = nullptr,
                           const std::string &threadName = std::string());
    ~ThreadHandler() override;

    uint64_t openDateTime() const;

    uint64_t closedDateTime() const;

protected:
    void initializeThread() override;

    HandlerBase *handler();
    void setHandler(HandlerBase *handler);

    uint32_t reconnectTimeout();

    uint64_t nextTryToConnect();

    PacketFactoryBase *packetFactory();

    MemoryBlock::Ptr &inputStream();
    MemoryBlock::Ptr &outputStream();

    void clearBuffers();

    void terminateChildThreads() override;
    void onBeforeWaitEvents() override;

    virtual void onConnecting(const HandlerBase *handler);
    virtual void onConnected(const HandlerBase *handler);
    virtual void onDisconnected(const HandlerBase *handler);

    virtual void onReadData(const HandlerBase *handler,
                            const void *data,
                            const int &size);

    virtual void onWriteData(const HandlerBase *handler,
                             const void *data,
                             const int &size);

    virtual bool onPacketReceived(const PacketBase::Ptr &) = 0;
    virtual bool onPacketSent(const PacketBase::Ptr &) = 0;

    virtual bool sendPacket(const PacketBase::Ptr &packet);

    uint64_t lastPacketSent() const;
    uint64_t lastPacketReceived() const;

    TrafficCounter *inputTrafficCounter();
    TrafficCounter *outputTrafficCounter();

    uint64_t connectedTickCount() const;

    void onConnectionStateChanged(HandlerBase *handler,
                                  HandlerBase::ConnectionState oldState,
                                  HandlerBase::ConnectionState newState) override;

    void onReadyRead(HandlerBase *handler) override;

    void onReadyWrite(HandlerBase *handler) override;

private:
    HandlerBase *_handler;
    uint32_t _reconnectTimeout;
    uint64_t _nextTryToConnect;

    MemoryBlock::Ptr _inputBuffer;
    MemoryBlock::Ptr _outputBuffer;

    MemoryBlock _localInputBuffer;

    PacketFactoryBase *_packetFactory;

    uint64_t _lastPacketSent;
    uint64_t _lastPacketReceived;

    TrafficCounter _inputTrafficCounter;
    TrafficCounter _outputTrafficCounter;

    uint64_t _connectedTickCount;

    uint64_t _openDateTime;
    uint64_t _closedDateTime;
};

}
