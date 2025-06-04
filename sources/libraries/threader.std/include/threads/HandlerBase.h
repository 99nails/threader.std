#pragma once

#include "MemoryBlock.h"
#include "ThreadsCommon.h"

#ifdef linux
#include "PollingPosix.h"
#endif

#ifdef _WIN32
#include "PollingsWindows.h"
#endif

// #include "../threader_global.h"

// #include <QObject>
// #include <QtGlobal>

#include <vector>

namespace Threader::Threads {

using namespace Threader::Utils;

// using ByteVector = std::vector<uint8_t>;

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
    explicit HandlerBase(const std::string &deviceName = "",
                         const Descriptor descriptor = INVALID_DESCRIPTOR);

#ifdef _WIN32
    Descriptor descriptor() const;
    virtual void setDescriptor(const Descriptor &value);
#endif

#ifdef linux
    void setDescriptor(const Descriptor &descriptor) override;
#endif

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
    virtual int write(const char *data, int count);
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
    std::string _deviceName;
    ConnectionState _connectionState;
    bool _needsToWrite;

#ifdef _WIN32
    Descriptor _descriptor;
#endif

// signals:
//     void signalOnConnectionStateChanged(Threader::Threads::HandlerBase *sender,
//                                         Threader::Threads::HandlerBase::ConnectionState oldState,
//                                         Threader::Threads::HandlerBase::ConnectionState newState);

//     void signalOnReadyRead(Threader::Threads::HandlerBase *sender);

//     void signalOnReadyWrite(Threader::Threads::HandlerBase *sender);

//     void signalOnError(Threader::Threads::HandlerBase *sender, int errorCode, QString errorDescription = QString(""));
};


using HandlerVector = std::vector<HandlerBase*>;

}
