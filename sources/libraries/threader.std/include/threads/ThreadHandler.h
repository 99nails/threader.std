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
                           const uint &reconnectTimeout = 0,
                           PacketFactoryBase *packetFactory = nullptr,
                           const std::string &threadName = std::string(),
                           const bool &portionedIO = false);
    ~ThreadHandler() override;

    const uint64_t openDateTime() const;

    const uint64_t closedDateTime() const;

protected:
    void initializeThread() override;

    HandlerBase *handler();
    void setHandler(HandlerBase *handler);

    uint reconnectTimeout();

    uint64_t nextTryToConnect();

    PacketFactoryBase *packetFactory();

    MemoryBlock::Ptr &inputStream();
    MemoryBlock::Ptr &outputStream();

    void clearBuffers();

    void terminateChildThreads() override;
    void onBeforeWaitEvents() override;

    virtual void onConnecting(){}
    virtual void onConnected(){}
    virtual void onDisconnected(){}

    virtual void onReadData(const void *, const int &){}
    virtual void onWriteData(const void *, const int &){}

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

    // void onError(HandlerBase *handler,
    //              int errorCode,
    //              std::string errorDescription) override;
private:
    HandlerBase *_handler;
    uint _reconnectTimeout;
    uint64_t _nextTryToConnect;

    MemoryBlock::Ptr _inputBuffer;
    MemoryBlock::Ptr _outputBuffer;

    MemoryBlock _localInputBuffer;

    PacketFactoryBase *_packetFactory;

    uint64_t _lastPacketSent;
    uint64_t _lastPacketReceived;

    TrafficCounter _inputTrafficCounter;
    TrafficCounter _outputTrafficCounter;

    bool _portionedIO;

    uint64_t _connectedTickCount;

    uint64_t _openDateTime;
    uint64_t _closedDateTime;

};

}
