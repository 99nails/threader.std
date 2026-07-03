#pragma once

#include "HandlerBase.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef linux
#include <termios.h>
#endif


namespace Threader::Threads {

class HandlerSerialPort : public HandlerBase
{
public:
    explicit HandlerSerialPort(HandlerDataConsumer *consumer,
                               const std::string &deviceName
#ifdef linux
                               , const bool exclusive = false
#endif
                               );

    bool open() override;

    void close() override;

    int inputBytesAvailable() override;

    int outputBytesAvailable() override;

#ifdef WIN32
    DescriptorsVector *events() override;

    bool process(Descriptor eventToProcess) override;

    int read(char *data, int count) override;

    int write(const char *data, int count) override;
#endif

#ifdef linux
    void assign(pollfd &event) override;

    bool process(const pollfd &event) override;
#endif

    uint32_t baudRate() const;
    bool enableParity() const;
    bool twoStopBits() const;
    uint32_t dataSize() const;

    void setBaudRate(const uint32_t &baudRate);
    void setEnableParity(bool enableParity);
    void setTwoStopBits(bool twoStopBits);
    void setDataSize(const uint32_t &dataSize);

#ifdef WIN32
    bool useOverlappedIO() const;
    void setUseOverlappedIO(bool useOverlappedIO);
#endif

private:
    uint32_t _baudRate;
    bool _enableParity;
    bool _twoStopBits;
    uint32_t _dataSize;
#ifdef linux
    bool _exclusive;
#endif

#ifdef WIN32
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
