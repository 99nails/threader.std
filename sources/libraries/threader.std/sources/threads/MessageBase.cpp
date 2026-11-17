#include "MessageBase.h"

#include "DateTimeUtils.h"
#include "StdStringUtils.h"
#include "ThreadUtils.h"

// #include <chrono>
// #include <ctime>

namespace Threader {

namespace Threads {

using namespace Threader::Utils;

// using namespace std::chrono;

MessageBase::MessageBase(const std::string &name)
    : _name(name)
{
    initialize();
}

MessageBase::MessageBase(const char name[])
    : _name(std::string(name))
{
    initialize();
}

std::string MessageBase::toString()
{
    return format("Message <%s> created %s in thread %d",
                  _name.c_str(),
                  DateTimeUtils::msecsSinceEpochToStdString(_created, true).c_str(),
                  _threadId);
}

std::string MessageBase::name() const
{
    return _name;
}

uint64_t MessageBase::created() const
{
    return _created;
}

THREAD_ID MessageBase::threadId() const
{
    return _threadId;
}

void MessageBase::initialize()
{
//    const auto now = std::chrono::system_clock::now();
    _created = DateTimeUtils::msecsSinceEpochLocal();
    // _created = std::chrono::system_clock::to_time_t(now);
    // auto clock = std::chrono::steady_clock::now();

    _threadId = ThreadUtils::currentThreadId();

}

void MessageBase::setName(const std::string &name)
{
    _name = name;
}

} // namespace Threads
} // namespace Threader
