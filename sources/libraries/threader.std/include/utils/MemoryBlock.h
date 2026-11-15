#pragma once

#include <stdlib.h>

#include <memory>

namespace Threader::Utils {

class MemoryBlock
{
public:
    using Ptr = std::shared_ptr<MemoryBlock>;

public:
    explicit MemoryBlock();
    explicit MemoryBlock(size_t capacity);
    explicit MemoryBlock(const void* data,
                const size_t size);

    explicit MemoryBlock(MemoryBlock &other);

    virtual ~MemoryBlock();

    size_t capacity() const;
    void setCapacity(size_t capacity);

    size_t size() const;

    void *data() const;

    void clear(bool release = false);

    void append(const void *data,
                const size_t size);

    size_t remove(size_t from,
                  size_t count,
                  bool setToZero = true);

protected:
    void setSize(size_t size);

private:
    size_t _capacity;
    size_t _size;
    void* _data;
};

}
