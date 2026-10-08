#include "PollingWindows.h"

#ifdef WIN32

#include "DateTimeUtils.h"

#include <algorithm>

namespace Threader::Threads {

using namespace Threader::Utils;

PollerBase::PollerBase(const DescriptorsVector &events)
    : _events(events)
{
}

PollerBase::~PollerBase()
{
    for (const auto descriptor : _events)
        CloseHandle(descriptor);
}

bool PollerBase::hasEvent(Descriptor event)
{
    for (const auto descriptor : _events)
        if (descriptor == event)
            return true;
    return false;
}

DescriptorsVector *PollerBase::events()
{
    return &_events;
}

void PollerBase::assign(const DescriptorsVector &events)
{
    _events = events;
//    _events.clear();
//    _events.push_back(events);
}

bool PollerBase::process(Descriptor eventToProcess)
{
    bool result = hasEvent(eventToProcess);
    return result;
}


Polling::Polling(const std::string &threadName)
    : _waitCount(0)
    , _threadName(threadName)
{
}

int Polling::pollersCount()
{
    return _pollers.size();
}

int Polling::registerPoller(PollerBase *poller)
{
    // проверка существования голосующего с добавляемым дескриптором
    auto found {std::find(begin(_pollers), end(_pollers), poller)};

    // если голосующий не найден
    if (found == end(_pollers))
    {
        // добавление нового голосущего
        _pollers.push_back(poller);
    }

    // получение количества голосующих
    int count = _pollers.size();

    return count;
}

int Polling::unregisterPoller(PollerBase *poller)
{
    // удаление голосующего
    for (auto iterator = _pollers.end(); iterator != _pollers.begin(); iterator--)
        _pollers.erase(iterator);

    // получение количества голосующих
    int count = _pollers.size();

    return count;
}

int Polling::poll(const uint32_t &timeout)
{
    // заполнение массива с данными голосующих
    DescriptorsVector pollArray;
    for (auto poller : _pollers)
    {
        DescriptorsVector events = *poller->events();
        for (auto event : events)
        {
            pollArray.push_back(event);
        }
    }

    auto pollArrayCount = static_cast<uint32_t>(pollArray.size());

    // запоминание начала ожидания событий
    auto waitStarted = DateTimeUtils::getTickCount();

    // ожидание голосования
    auto waitResult = WaitForMultipleObjects(pollArrayCount,
                                             pollArray.data(),
                                             false,
                                             timeout);

    // фиксация общего времени ожидания событий
    waitStarted = DateTimeUtils::getTickCount() - waitStarted;
    _waitCount = _waitCount + waitStarted;

    // выход по таймауту
    if (WAIT_TIMEOUT == waitResult)
        return 0;

    // выход по ошибке
    if (WAIT_FAILED == waitResult)
        return -1;

    if (pollArrayCount > waitResult)
    {
        // получение дескриптора события
        int eventIndex = waitResult - WAIT_OBJECT_0;
        Descriptor eventToProcess = pollArray.at(eventIndex);
        // сброс события раньше был здесь
        // ResetEvent(eventToProcess);
        // обработка события в голосующем
        for (auto poller : _pollers)
            if (poller->hasEvent(pollArray.at(eventIndex)))
                poller->process(eventToProcess);

        // сброс события теперь здесь после обработки события
        // ResetEvent(eventToProcess);
    }

    return 1;
}

int64_t Polling::waitCount() const
{
    return _waitCount;
}

const std::string &Polling::threadName() const
{
    return _threadName;
}

void Polling::setThreadName(const std::string &threadName)
{
    _threadName = threadName;
}

}

#endif
