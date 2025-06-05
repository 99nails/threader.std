#include "PacketFactoryBase.h"

#include "DateTimeUtils.h"

#include <string.h>

namespace Threader {

namespace Threads {


PacketBase::PacketBase(const char *data,
                       const int size)
    : _createdUtc(DateTimeUtils::msecsSinceEpochLocal())
    , _createdTickCount(DateTimeUtils::getTickCount())
    , _data(nullptr)
{
    setData(data, size);
}

PacketBase::PacketBase(int size)
    : _createdUtc(DateTimeUtils::msecsSinceEpochLocal())
    , _createdTickCount(DateTimeUtils::getTickCount())
    , _data(nullptr)
{
    _data = calloc(1, size);
}

PacketBase::PacketBase(const MemoryBlock::Ptr &data)
    : _createdUtc(DateTimeUtils::msecsSinceEpochLocal())
    , _createdTickCount(DateTimeUtils::getTickCount())
    , _data(nullptr)
{
    if (data)
    {
        _data = calloc(1, data->size());
        memcpy(_data, data->data(), data->size());
    }
}

PacketBase::~PacketBase()
{
    clear();
}

void PacketBase::clear()
{
    if (_data)
    {
        free(_data);
        _data = nullptr;
    }
}

uint64_t PacketBase::created() const
{
    return _createdUtc;
}

uint64_t PacketBase::createdTickCount() const
{
    return _createdTickCount;
}

void* PacketBase::data() const
{
    return _data;
}

void PacketBase::setData(const void *data, const size_t size)
{
    clear();

    if (data)
    {
        _data = calloc(1, size);
        memcpy(_data, data, size);
    }
}

void PacketBase::setData(const MemoryBlock::Ptr &data)
{
    clear();

    if (data)
    {
        setData(data->data(), data->size());
    }
}

void PacketFactoryBase::setLastResult(int value)
{
    _lastResult = value;
}

int PacketFactoryBase::lastResult() const
{
    return _lastResult;
}

}}
