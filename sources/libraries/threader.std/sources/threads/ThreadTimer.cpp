
#include "ThreadTimer.h"

#include "MessageTimer.h"

namespace Threader::Threads
{

const std::string ThreadTimer::THREAD_NAME_PREFFIX = "Thread.Timer.";

ThreadTimer::ThreadTimer(MessageConsumer *parent,
                         const std::string &timerName,
                         const uint32_t &delayMsecs,
                         const bool multiShot)
    : ThreadBase(parent, THREAD_NAME_PREFFIX + timerName)
    , _timerName(timerName)
    , _delay(delayMsecs)
    , _multiShot(multiShot)
    , _shotCount(0)
{
    setTimeout(_delay);
}

std::string ThreadTimer::timerName() const
{
    return _timerName;
}

int ThreadTimer::delay() const
{
    return _delay;
}

bool ThreadTimer::multiShot() const
{
    return _multiShot;
}

void ThreadTimer::asyncSetTimeout(const int &delayMsecs)
{
    postMessage1<MessageTimerSetTimeout>("WakeUp", delayMsecs);
}

void ThreadTimer::processMessage(const MessageBase::Ptr &message)
{
    if (auto msg = std::dynamic_pointer_cast<MessageTimerSetTimeout>(message))
        setTimeout(msg->delay());

    return;
}

void ThreadTimer::onIdle()
{
    sendTimerMessage();
    if (!_multiShot)
        terminateThread();
}

void ThreadTimer::sendTimerMessage()
{
    if (!parentThread())
        return;
    parentThread()->postMessage1<MessageTimer>(_timerName, _shotCount);
    _shotCount++;
}

}
