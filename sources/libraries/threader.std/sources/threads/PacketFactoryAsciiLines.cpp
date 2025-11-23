#include "PacketFactoryAsciiLines.h"

namespace Threader {

namespace Threads {

/******************************************************************************
 * PacketAsciiLines
 *****************************************************************************/

PacketAsciiLine::PacketAsciiLine(const std::string &line)
    : PacketBase(0)
    , _line(line)
{
}

std::string PacketAsciiLine::line() const
{
    return _line;
}

bool PacketAsciiLine::write(MemoryBlock::Ptr &data) const
{
    std::string packetString =  _line + CRLF;
    data->append(packetString.c_str(), packetString.length());
    return true;
}


/******************************************************************************
 * PacketFactoryAsciiLines
 *****************************************************************************/

PacketFactoryAsciiLines::PacketFactoryAsciiLines(const std::string &lineTerminator)
    : PacketFactoryBase()
    , _lineTerminator(lineTerminator)
{
}

PacketBase::Ptr PacketFactoryAsciiLines::tryExtractPacket(MemoryBlock::Ptr &data)
{
    if (!data || data->size() == 0)
        return nullptr;

    auto dataString = std::string((char*)data->data());
    auto position = dataString.find(_lineTerminator);
    if (std::string::npos == position || position >= dataString.length())
        return nullptr;

    data->remove(0, position + _lineTerminator.length());

    auto line = StdStringUtils::rtrim(dataString.substr(0, position));
    return std::make_shared<PacketAsciiLine>(line);
}

PacketBase::Ptr PacketFactoryAsciiLines::buildPacket(const void *data,
                                                     bool generatePacketId)
{
    std::string stringData((char*)data);
    return std::make_shared<PacketAsciiLine>(stringData);

    (void)(generatePacketId);
}

}}
