#pragma once

#include <list>
#include <stdint.h>

namespace Threader::Utils {

struct TrafficHistory
{
    TrafficHistory(const uint64_t &received = 0,
                   const uint64_t &count = 0)
        : Received(received)
        , Count(count)
    {
    }

    uint64_t Received;
    uint64_t Count;
};

using TrafficHistoryList = std::list<TrafficHistory>;


class TrafficCounter
{
public:
    explicit TrafficCounter(int secondsToSore = 5);
    void reset();
    uint64_t append(const uint64_t &dateTime,
                    const uint64_t &count);
    uint64_t actualize();
    uint64_t currentSpeed() const;

    int secondsToStore() const;
    void setSecondsToStore(int secondsToStore);

    uint64_t total() const;

private:
    int _secondsToStore;
    uint64_t _total;
    uint64_t _currentSpeed;
    TrafficHistoryList _historyList;
};

}
