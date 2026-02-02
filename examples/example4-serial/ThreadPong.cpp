#include "ThreadPong.h"
#include "Example4Common.h"
#include "HandlerSerialPort.h"
#include "PacketFactoryAsciiLines.h"

ThreadPong::ThreadPong(const std::string &deviceName)
    : ThreadHandler(nullptr,
                    new HandlerSerialPort(nullptr, deviceName),
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

void ThreadPong::onConnecting(const HandlerBase *handler)
{
    if (!handler)
        return;
    writeLog(Message210, handler->deviceName().c_str());
}

void ThreadPong::onConnected(const HandlerBase *handler)
{
    if (!handler)
        return;
    writeLog(Message211, handler->deviceName().c_str());
}

void ThreadPong::onDisconnected(const HandlerBase *handler)
{
    if (!handler)
        return;
    writeLog(Message213, handler->deviceName().c_str());
}

void ThreadPong::onReadData(const HandlerBase *handler,
                            const void *,
                            const int &count)
{
    if (!handler)
        return;
    writeLog(Message214, count, handler->deviceName().c_str());
}

void ThreadPong::onWriteData(const HandlerBase *handler,
                             const void *,
                             const int &count)
{
    if (!handler)
        return;
    writeLog(Message215, count, handler->deviceName().c_str());
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

    (void)errorDescription;
}

bool ThreadPong::onPacketReceived(const PacketBase::Ptr &packet)
{
    if (!packet)
        return false;

    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);
    writeLog(Message216, packetAscii->line().c_str(), handler()->deviceName().c_str());

     sendPacket(packet);
    return true;
}

bool ThreadPong::onPacketSent(const PacketBase::Ptr &packet)
{
    if (!packet)
        return false;

    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);
    writeLog(Message217, packetAscii->line().c_str(), handler()->deviceName().c_str());

    return true;
}
