#pragma once

#include <mutex>
#include <thread>

namespace Threader {

namespace Utils {

class StdThreadWrapper
{
public:
    StdThreadWrapper();
    virtual ~StdThreadWrapper();

    bool isRunning() const;
    bool isTerminated() const;
    bool isFinished() const;

    void start();
    void stop();
    void terminate();

protected:
    void setIsRunning(const bool isRunning);

    virtual void run() = 0;
private:
    std::thread *_thread = nullptr;

    bool _isRunning;
    std::mutex *_mutexIsRunning;

    bool _isTerminated;
    std::mutex *_mutexIsTerminated;
};

} // namespace Utils

} // namespace Threader
