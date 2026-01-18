#include "StdThreadWrapper.h"

namespace Threader::Utils
{

StdThreadWrapper::StdThreadWrapper()
    : _wasRunning(false)
    , _isRunning(false)
    , _mutexIsRunning(new std::mutex())
    , _isTerminated(false)
    , _mutexIsTerminated(new std::mutex())
{}

StdThreadWrapper::~StdThreadWrapper()
{
    delete _mutexIsRunning;
    delete _mutexIsTerminated;

    if (_thread)
    {
        delete _thread;
        _thread = nullptr;
    }
}

bool StdThreadWrapper::wasStarted() const
{
    return _thread;
}

bool StdThreadWrapper::isThreadRunning() const
{
    std::lock_guard<std::mutex> lock(*_mutexIsRunning);
    return _isRunning;
}

bool StdThreadWrapper::isThreadTerminated() const
{
    std::lock_guard<std::mutex> lock(*_mutexIsTerminated);
    return _isTerminated;
}

bool StdThreadWrapper::isThreadFinished() const
{
    return !isThreadRunning() && (_thread || _wasRunning);
}

void StdThreadWrapper::startThread()
{
    if (_thread)
        return;

    auto threadFunction = [this](){
        setIsRunning(true);

        execute();

        setIsRunning(false);
    };

    _thread = new std::thread(threadFunction);
    _thread->detach();
}

void StdThreadWrapper::terminateThread()
{
    std::lock_guard<std::mutex> lock(*_mutexIsTerminated);
    _isTerminated = true;
}

void StdThreadWrapper::setIsRunning(const bool isRunning)
{
    std::lock_guard<std::mutex> lock(*_mutexIsRunning);
    _isRunning = isRunning;
    if (_isRunning)
        _wasRunning = true;
}

}
