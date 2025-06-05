#include "PacketFactoryAsciiLines.h"

namespace Threader {

namespace Threads {

/******************************************************************************
 * PacketAsciiLines
 *****************************************************************************/

PacketAsciiLines::PacketAsciiLines(const StdStringList &lines)
    : PacketBase(0)
    , _lines(lines)
{
}

StdStringList PacketAsciiLines::lines() const
{
    return _lines;
}

bool PacketAsciiLines::write(MemoryBlock::Ptr &data) const
{
    std::string packetString = "";
    for (const auto &line : _lines)
        packetString += line + PacketFactoryAsciiLines::CRLF;
    data->append(packetString.c_str(), packetString.length());
    return true;
}


/******************************************************************************
 * PacketFactoryAsciiLines
 *****************************************************************************/

const char PacketFactoryAsciiLines::CRLF[] = "\r\n";

PacketFactoryAsciiLines::PacketFactoryAsciiLines()
    : PacketFactoryBase()
{
}

PacketBase::Ptr PacketFactoryAsciiLines::tryExtractPacket(MemoryBlock::Ptr &data)
{
    int conveyorPosition = 0;
    int conveyorLength = data->size();
    int deleteToPosition = -1;
    std::string currentLine = "";
    StdStringList lines;

    char* dataPointer = (char*)data->data();

    while (conveyorPosition < conveyorLength) {
        char currentChar = dataPointer[conveyorPosition];

        if (CRLF[0] != currentChar && CRLF[1] != currentChar) {
            currentLine += currentChar;
        }
        else
        {
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
                currentLine.clear();
            }
            deleteToPosition = conveyorPosition;
        }

        conveyorPosition++;
    }

    if (deleteToPosition >= 0) {
        data->remove(0, deleteToPosition + 1);
    }

    if (lines.size() > 0) {
        return std::make_shared<PacketAsciiLines>(lines);
    }

    return nullptr;
}

PacketBase::Ptr PacketFactoryAsciiLines::buildPacket(const void *data,
                                                     bool generatePacketId)
{
    std::string stringData((char*)data);
    return std::make_shared<PacketAsciiLines>(StdStringList({stringData}));

    (void)(generatePacketId);
}


}}
