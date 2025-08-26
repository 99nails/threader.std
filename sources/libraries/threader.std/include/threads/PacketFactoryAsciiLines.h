
#pragma once

#include "PacketFactoryBase.h"

#include "StdStringUtils.h"

namespace Threader::Threads {

using namespace Threader::Utils;

class PacketAsciiLine : public PacketBase
{
public:
    using Ptr = std::shared_ptr<PacketAsciiLine>;

public:
    explicit PacketAsciiLine(const std::string &line = std::string());
    virtual ~PacketAsciiLine() = default;

    bool write(MemoryBlock::Ptr &data) const override;

    std::string line() const;

protected:
    std::string _line;

};


constexpr char CR[] = "\r";
constexpr char LF[] = "\n";
constexpr char CRLF[] = "\r\n";

class PacketFactoryAsciiLines : public PacketFactoryBase
{
public:
    explicit PacketFactoryAsciiLines(const std::string &lineTerminator = CRLF);

    PacketBase::Ptr tryExtractPacket(MemoryBlock::Ptr &data) override;
    PacketBase::Ptr buildPacket(const void *data,
                                bool generatePacketId = false) override;

private:
    std::string _lineTerminator;
};

}
