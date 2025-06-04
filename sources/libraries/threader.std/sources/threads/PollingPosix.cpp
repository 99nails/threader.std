
#include "PollingPosix.h"

#include "DateTimeUtils.h"

#ifdef linux

namespace Threader {

namespace Threads {

using namespace Threader::Utils;

PollerBase::PollerBase(const int &descriptor, const short int events)
    : _descriptor(descriptor)
    , _events(events)
{
}

int PollerBase::descriptor() const
{
    return _descriptor;
}

void PollerBase::setDescriptor(const Descriptor &descriptor)
{
    _descriptor = descriptor;
}

short int PollerBase::events()
{
    return _events;
}

void PollerBase::setEvents(short int events)
{
    _events = events;
}

void PollerBase::assign(pollfd &event)
{
    event.fd = _descriptor;
    event.events = _events;
    event.revents = 0;
}

bool PollerBase::process(const pollfd &event)
{
    bool result = event.fd == _descriptor;

    // if (result)
    //     emit signalOnPollEvent(this, event);

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
    int index = -1;
    if (poller->descriptor() != INVALID_DESCRIPTOR)
        index = indexOfDescriptor(poller->descriptor());

    // если дескриптор найден
    if (index >= 0)
    {
        // замена голосующего
        return -1;
    }
    else
    {
        // добавление нового голосущего
        _pollers.push_back(poller);
    }

    // получение количества голосующих
    int count = _pollers.size();

    // поддержание размерности массива структур с данными голосующих
    _pollStructArray.resize(count);

    return count;
}

int Polling::unregisterPoller(PollerBase *poller)
{
    // удаление голосующего
    for (auto iterator = _pollers.end(); iterator != _pollers.begin(); iterator--)
        _pollers.erase(iterator);

    // получение количества голосующих
    int count = _pollers.size();

    // поддержание размерности массива структур с данными голосующих
    _pollStructArray.resize(count);

    return count;
}

int Polling::poll(const uint32_t &timeout)
{
    // получение количества голосующих
    int count = _pollStructArray.size();

    // проверка существования голосующих
    if (0 == count)
        return 0;

    // формирование ссылки на массив структур голосования
    pollfd *pollArrayPointer = _pollStructArray.data();

    // заполнение массива структур с данными голосующих
    for (int i = 0; i < count; i++)
        _pollers.at(i)->assign(pollArrayPointer[i]);

    int result;
    // голосование
    do
    {
        // запоминание начала ожидания
        auto pollStart = DateTimeUtils::getTickCount();
        // ожидание
        result = ::poll(pollArrayPointer,
                        static_cast<uint32_t>(count),
                        static_cast<int32_t>(timeout));
        // вычисление времени в ожидании
        _waitCount += DateTimeUtils::getTickCount() - pollStart;
        // может прийти сигнал и тогда ошибка будет EINTR
        // в этом случае ожидание продолжается
    } while (result < 0 && EINTR == errno);

    // обработка результатов
    if (result != 0)
        for (int i = 0; i < count; i++)
        {
            // если произошло событие
            if (pollArrayPointer[i].revents != 0)
                // обработка события голосующим
                _pollers.at(i)->process(pollArrayPointer[i]);
        }

    return result;
}

int64_t Polling::waitCount() const
{
    return _waitCount;
}

int Polling::indexOfDescriptor(const int &descriptor)
{
    for (int i = 0; i < _pollers.size(); i++)
        if (_pollers.at(i)->descriptor() == descriptor)
            return i;
    return -1;
}

const std::string &Polling::threadName() const
{
    return _threadName;
}

void Polling::setThreadName(const std::string &threadName)
{
    _threadName = threadName;
}

}}

#endif
