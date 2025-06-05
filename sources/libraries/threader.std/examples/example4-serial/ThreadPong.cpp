#include "ThreadPong.h"
#include "Example4Common.h"
#include "HandlerSerialPort.h"
#include "PacketFactoryAsciiLines.h"

ThreadPong::ThreadPong()
    : ThreadHandler(nullptr,
                    new HandlerSerialPort(nullptr, "/dev/pts/2"),
                    1000,
                    new PacketFactoryAsciiLines(),
                    "Thread.Pong")
{

}

void ThreadPong::onThreadStarted()
{
    writeLog(Message200);
}

void ThreadPong::onThreadFinishing()
{
    writeLog(Message201);
}

void ThreadPong::onThreadFinished()
{
    writeLog(Message202);
}

void ThreadPong::onConnecting()
{
    if (!handler())
        return;
    writeLog(Message210, handler()->deviceName().c_str());
}

void ThreadPong::onConnected()
{
    if (!handler())
        return;
    writeLog(Message211, handler()->deviceName().c_str());
}

void ThreadPong::onDisconnected()
{
    if (!handler())
        return;
    writeLog(Message213, handler()->deviceName().c_str());
}

void ThreadPong::onReadData(const void *, const int &count)
{
    if (!handler())
        return;
    writeLog(Message214, count, handler()->deviceName().c_str());
}

void ThreadPong::onWriteData(const void *, const int &count)
{
    if (!handler())
        return;
    writeLog(Message215, count, handler()->deviceName().c_str());
}

void ThreadPong::onError(HandlerBase *handler,
                         const int errorCode,
                         const std::string &errorDescription)
{
    if (!handler)
        return;
    writeLog(Message212,
             handler->deviceName().c_str(),
             errorString(errorCode).c_str());

}

bool ThreadPong::onPacketReceived(const PacketBase::Ptr &packet)
{
    if (!packet)
        return false;

    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLines>(packet);
    writeLog(Message216, packetAscii->lines().front().c_str(), handler()->deviceName().c_str());

    // sendPacket(packet);
    return true;
}

bool ThreadPong::onPacketSent(const PacketBase::Ptr &packet)
{
    if (!packet)
        return false;

    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLines>(packet);
    writeLog(Message217, packetAscii->lines().front().c_str(), handler()->deviceName().c_str());

    return true;
}
