#include "MessageTimer.h"

namespace Threader::Threads
{

MessageTimer::MessageTimer(const std::string &name, const uint64_t &shotCount)
    : MessageBase(name), _shotCount(shotCount)
{
}

uint64_t MessageTimer::shotCount() const { return _shotCount; }

MessageTimerSetTimeout::MessageTimerSetTimeout(const std::string &name, const int &newDelay)
    : MessageBase(name), _delay(newDelay)
{
}

}
