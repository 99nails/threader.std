#include "StdThreadWrapper.h"

namespace Threader {
namespace Utils {

Threader::Utils::StdThreadWrapper::StdThreadWrapper()
    : _isRunning(false)
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

bool StdThreadWrapper::isRunning() const
{
    std::lock_guard<std::mutex> lock(*_mutexIsRunning);
    return _isRunning;
}

bool StdThreadWrapper::isTerminated() const
{
    std::lock_guard<std::mutex> lock(*_mutexIsTerminated);
    return _isTerminated;
}

bool StdThreadWrapper::isFinished() const
{
    return !isRunning() && _thread;
}

void StdThreadWrapper::start()
{
    if (_thread)
        return;

    auto threadFunction = [this](){
        setIsRunning(true);

        run();

        setIsRunning(false);
    };

    _thread = new std::thread(threadFunction);
    _thread->detach();
}

void StdThreadWrapper::terminate()
{
    std::lock_guard<std::mutex> lock(*_mutexIsTerminated);
    _isTerminated = true;
}

void StdThreadWrapper::setIsRunning(const bool isRunning)
{
    std::lock_guard<std::mutex> lock(*_mutexIsRunning);
    _isRunning = isRunning;
}

} // namespace Utils
} // namespace Threader
