#pragma once

#include "MemoryBlock.h"
#include "ThreadsCommon.h"

#ifdef linux
#include "PollingPosix.h"
#endif

#ifdef _WIN32
#include "PollingWindows.h"
#endif

// #include "../threader_global.h"

// #include <QObject>
// #include <QtGlobal>

#include <string>
#include <vector>

namespace Threader::Threads {

using namespace Threader::Utils;

class HandlerDataConsumer;


class HandlerBase : public PollerBase
{
    // Q_OBJECT
public:
    /**
     * @brief ConnectionState состояние подключения
     */
    enum class ConnectionState
    {
        Disconnected,
        Connecting,
        Connected
    };

    /**
     * @brief HandlerBase Конструктор с именем устройства
     * @param deviceName Имя устройства в системе
     */
    explicit HandlerBase(HandlerDataConsumer *consumer = nullptr,
                         const std::string &deviceName = "",
                         const Descriptor descriptor = INVALID_DESCRIPTOR);

#ifdef _WIN32
    Descriptor descriptor() const;
    virtual void setDescriptor(const Descriptor &value);
#endif

#ifdef linux
    void setDescriptor(const Descriptor &descriptor) override;
#endif

    HandlerDataConsumer *consumer() const;
    bool setConsumer(HandlerDataConsumer *consumer);

    ConnectionState connectionState();
    virtual bool isConnected();

    std::string deviceName() const;
    void setDeviceName(const std::string &deviceName);

    bool needsToWrite() const;
    void setNeedsToWrite(const bool needsToWrite);

    virtual bool open() = 0;
    virtual void close() = 0;

    /**
     * @brief inputBytesAvailable - Возврат количества байт во входном буфере
     * @return - Количество байт во входном буфере
     */
    virtual int inputBytesAvailable() = 0;

    /**
     * @brief outputBytesAvailable - Возврат количества свободных байт в выходном буфере
     * @return - Количество байт в выходном буфере
     */
    virtual int outputBytesAvailable() = 0;

    /**
     * @brief read - Чтение данных из устройства
     * @param data - указатель на буфер для чтения данных
     * @param count - размер буфера
     * @return - количество прочитанных данных
     */
#ifdef linux
    virtual int read(char *data, int count);
    virtual int write(const void *data, int count);
#endif
#ifdef _WIN32
    virtual int read(char *data, int count) = 0;
    virtual int write(const char *data, int count) = 0;
#endif    

    std::string readString();
    int writeString(const std::string &data);

    MemoryBlock::Ptr readBytes();
    int writeBytes(const MemoryBlock::Ptr &data);

protected:
    void setConnectionState(ConnectionState value);

    void doReadyRead();
    void doReadyWrite();
    void doError(int errorCode, std::string errorDescription = std::string());

private:
    HandlerDataConsumer *_consumer;
    std::string _deviceName;
    ConnectionState _connectionState;
    bool _needsToWrite;

#ifdef _WIN32
    Descriptor _descriptor;
#endif
};


using HandlerVector = std::vector<HandlerBase*>;

class HandlerDataConsumer
{
public:
    virtual void onConnectionStateChanged(HandlerBase *sender,
                                          const HandlerBase::ConnectionState oldState,
                                          const HandlerBase::ConnectionState newState)
        = 0;

    virtual void onReadyRead(HandlerBase *sender) = 0;

    virtual void onReadyWrite(HandlerBase *sender) = 0;

    virtual void onError(HandlerBase *sender,
                         const int errorCode,
                         const std::string &errorDescription = std::string())
        = 0;
};

}

