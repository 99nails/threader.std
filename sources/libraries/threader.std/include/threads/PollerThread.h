#pragma once

#include "ThreadsCommon.h"

#ifdef linux
#include "PollingPosix.h"
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef WIN32
#include "PollingWindows.h"
#include <stdint.h>
#endif

namespace Threader {

namespace Threads {

using Signal = uint8_t;

enum class ThreadSignals : Signal
{
    SignalTerminate = 1,
    SignalWakeUp = 2,
    SignalTerminateChildThreads = 3
};

class ISignalHandler
{
public:
    virtual void signalProcess(Signal signal) = 0;
};

class PollerThread : public PollerBase
{
public:
    explicit PollerThread(ISignalHandler *signalHandler = nullptr);
    ~PollerThread() override;

    void sendSignalTerminate();
    void sendSignalTerminateChildThreads();
    void sendSignalWakeUp();

#ifdef _WIN32
    bool process(Descriptor eventToProcess) override;
#endif
#ifdef linux
    bool process(const pollfd &event) override;
#endif

    ISignalHandler *signalHandler() const;
    void setSignalHandler(ISignalHandler *signalHandler);

private:
#ifdef linux
    Descriptor _pipeDescriptors[2];

    bool readSignal(Signal &signal);
#endif
    ISignalHandler *_signalHandler;

#ifdef _WIN32
    Descriptor _eventTerminate;
    Descriptor _eventTerminateChildThreads;
    Descriptor _eventWakeUp;
#endif

    void sendSignal(ThreadSignals signal);
};

} // namespace Threads
} // namespace Threader
