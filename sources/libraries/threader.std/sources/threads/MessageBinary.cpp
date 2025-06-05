#include "MessageBinary.h"

#include <memory>

namespace Threader {

namespace Threads {

const std::string MessageBinary::MESSAGE_NAME = "Message.Binary";

MessageBinary::MessageBinary(const void *data, size_t size)
    : MessageBase(MESSAGE_NAME)
{
    _data = std::make_shared<MemoryBlock>(data, size);
}

MessageBinary::MessageBinary(const std::string &name, const void *data, size_t size)
    : MessageBase(name)
{
    _data = std::make_shared<MemoryBlock>(data, size);
}

MessageBinary::MessageBinary(const std::string &name, const MemoryBlock::Ptr data)
    : MessageBase(name)
    , _data(data)
{}

const MemoryBlock::Ptr &MessageBinary::data() const
{
    return _data;
}

void MessageBinary::setData(MemoryBlock::Ptr data)
{
    _data = data;
}

} // namespace Threads
} // namespace Threader
