#pragma once

#include "MessageBase.h"
#include "MemoryBlock.h"

namespace Threader {

namespace Threads {

using namespace Threader::Utils;

class MessageBinary : public MessageBase
{
public:
    using Ptr = std::shared_ptr<MessageBinary>;

public:

    static const std::string MESSAGE_NAME;

    explicit MessageBinary(const void *data, size_t size);
    explicit MessageBinary(const std::string &name, const void *data, size_t size);
    explicit MessageBinary(const std::string &name, const MemoryBlock::Ptr data);

    const MemoryBlock::Ptr &data() const;

protected:
    void setData(MemoryBlock::Ptr data);

private:
    MemoryBlock::Ptr _data;
};

}}
