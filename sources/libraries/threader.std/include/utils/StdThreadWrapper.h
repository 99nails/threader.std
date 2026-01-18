#pragma once

#include <functional>
#include <mutex>
#include <thread>

namespace Threader::Utils
{

using ThreadFunction = std::function<void()>;

class StdThreadWrapper
{
public:
    StdThreadWrapper();
    virtual ~StdThreadWrapper();

    virtual bool wasStarted() const;
    virtual bool isThreadRunning() const;
    virtual bool isThreadTerminated() const;
    virtual bool isThreadFinished() const;

    virtual void startThread();
    virtual void terminateThread();

protected:
    virtual void setIsRunning(const bool isRunning);

    virtual void execute() = 0;
private:
    std::thread *_thread = nullptr;

    bool _wasRunning;
    bool _isRunning;
    std::mutex *_mutexIsRunning;

    bool _isTerminated;
    std::mutex *_mutexIsTerminated;
};

}
