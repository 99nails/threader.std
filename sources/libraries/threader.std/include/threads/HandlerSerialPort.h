#pragma once

#include "HandlerBase.h"

// #include <QObject>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef linux
#include <termios.h>
#endif


namespace Threader::Threads {

class HandlerSerialPort : public HandlerBase
{
    // Q_OBJECT
public:
    explicit HandlerSerialPort(HandlerDataConsumer *consumer,
                               const std::string &deviceName);

    bool open() override;

    void close() override;

    int inputBytesAvailable() override;

    int outputBytesAvailable() override;

#ifdef _WIN32
    DescriptorsVector *events() override;

    bool process(Descriptor eventToProcess) override;

    int read(char *data, int count) override;

    int write(const char *data, int count) override;
#endif

#ifdef linux
    void assign(pollfd &event) override;

    bool process(const pollfd &event) override;
#endif

    uint baudRate() const;
    bool enableParity() const;
    bool twoStopBits() const;
    uint dataSize() const;

    void setBaudRate(const uint &baudRate);
    void setEnableParity(bool enableParity);
    void setTwoStopBits(bool twoStopBits);
    void setDataSize(const uint &dataSize);

#ifdef _WIN32
    bool useOverlappedIO() const;
    void setUseOverlappedIO(bool useOverlappedIO);
#endif

private:
    uint _baudRate;
    bool _enableParity;
    bool _twoStopBits;
    uint _dataSize;

#ifdef _WIN32
    bool _useOverlappedIO;
    OVERLAPPED _overlapped;
    DWORD _mask;

    void initializeEvent();
    void registerEvent();
    void unregisterEvent();
    bool processEvent();
#endif
};


}
