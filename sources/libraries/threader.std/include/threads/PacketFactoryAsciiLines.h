
#pragma once

#include "PacketFactoryBase.h"

#include "StdStringUtils.h"

namespace Threader::Threads {

using namespace Threader::Utils;

class PacketAsciiLines : public PacketBase
{
public:
    using Ptr = std::shared_ptr<PacketAsciiLines>;

public:
    explicit PacketAsciiLines(const StdStringList &lines = {});
    virtual ~PacketAsciiLines() = default;

    bool write(MemoryBlock::Ptr &data) const override;

    StdStringList lines() const;

protected:
    StdStringList _lines;

};


class PacketFactoryAsciiLines : public PacketFactoryBase
{
public:
    static const char CRLF[];

    explicit PacketFactoryAsciiLines();

    PacketBase::Ptr tryExtractPacket(MemoryBlock::Ptr &data) override;
    PacketBase::Ptr buildPacket(const void *data,
                                bool generatePacketId = false) override;
};

}
