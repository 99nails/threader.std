#include "MemoryBlock.h"

#include <string.h>

#include <iostream>

namespace Threader::Utils {

MemoryBlock::MemoryBlock()
    : _capacity(0)
    , _size(0)
    , _data(nullptr)
{
}

MemoryBlock::MemoryBlock(size_t capacity)
    : _size(0)
{
    clear(true);
}

MemoryBlock::MemoryBlock(const void *data, const size_t size)
    : _size(size)
{
    setCapacity(size);
    if (_data)
        memcpy(_data, data, size);
}

MemoryBlock::MemoryBlock(MemoryBlock &other)
{
    append(other.data(), other.size());
}

MemoryBlock::~MemoryBlock()
{
    clear();
}

size_t MemoryBlock::capacity() const
{
    return _capacity;
}

void MemoryBlock::setCapacity(size_t capacity)
{
    if (0 == capacity)
    {
        clear(false);
        return;
    }

    _data = realloc(_data, capacity);
    if (capacity > _capacity)
    {
        auto from = (uint8_t*)_data + _capacity;
        memset(from, 0, capacity - _capacity);
    }

    if (_size > capacity)
        _size = capacity;

    _capacity = capacity;

    std::cout << "Capacity " << _capacity << std::endl;
}

size_t MemoryBlock::size() const
{
    return _size;
}

void *MemoryBlock::data() const
{
    return _data;
}

void MemoryBlock::clear(bool release)
{
    if (!_data)
        return;

    if (release)
    {
        free(_data);
        _data = nullptr;
        _capacity = 0;
    }

    _size = 0;
}

void MemoryBlock::append(const void *data,
                         const size_t size)
{
    auto previousSize = _size;
    setSize(previousSize + size);
    memcpy((uint8_t*)_data + previousSize, data, size);
}

size_t MemoryBlock::remove(size_t from,
                           size_t count)
{
    if (from > _size)
        return 0;

    if (from + count > _size)
        count = _size - from;

    memcpy((uint8_t*)_data + from,
           (uint8_t*)_data + from + count,
           _size - from - count);

    _size -= count;

    return _size;
}

void MemoryBlock::setSize(size_t size)
{
    if (size > capacity())
        setCapacity(size);
    _size = size;
}

}
