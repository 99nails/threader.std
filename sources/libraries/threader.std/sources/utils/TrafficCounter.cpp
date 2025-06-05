#include "TrafficCounter.h"

#include "DateTimeUtils.h"

namespace Threader::Utils {

TrafficCounter::TrafficCounter(int secondsToSore)
    : _secondsToStore(secondsToSore)
    , _total(0)
{    
}

void TrafficCounter::reset()
{
    _total = 0;
    _historyList.clear();
}

uint64_t TrafficCounter::append(const uint64_t &dateTime,
                                const uint64_t &count)
{
    _total += count;
    _historyList.push_back(TrafficHistory(dateTime, count));

    return actualize();
}

uint64_t TrafficCounter::currentSpeed() const
{
    return _currentSpeed;
}

int TrafficCounter::secondsToStore() const
{
    return _secondsToStore;
}

void TrafficCounter::setSecondsToStore(int secondsToStore)
{
    _secondsToStore = secondsToStore;
}

uint64_t TrafficCounter::total() const
{
    return _total;
}

uint64_t TrafficCounter::actualize()
{
    uint64_t limit = DateTimeUtils::msecsSinceEpochLocal() - _secondsToStore * 1000;

    int removeCount = 0;
    uint64_t trafficCount = 0;

    auto iterator = _historyList.begin();
    while (iterator != _historyList.end())
    {
        if (iterator->Received < limit)
            _historyList.erase(iterator++);
        else
        {
            trafficCount += iterator->Count;
            iterator++;
        }
    }

    _currentSpeed = trafficCount / _secondsToStore;

    return _currentSpeed;
}

}
