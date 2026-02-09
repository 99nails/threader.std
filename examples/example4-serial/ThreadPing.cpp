#include "ThreadPing.h"

#include "Example4Common.h"

#include "HandlerSerialPort.h"
#include "PacketFactoryAsciiLines.h"

ThreadPing::ThreadPing(const std::string &deviceName)
    : ThreadHandler(nullptr,
                    new HandlerSerialPort(nullptr, deviceName),
                    1000,
                    new PacketFactoryAsciiLines(),
                    "Thread.Ping")
{
}

void ThreadPing::onThreadStarted()
{
    writeLog(Message100);
}

void ThreadPing::onThreadFinishing()
{
    writeLog(Message101);
}

void ThreadPing::onThreadFinished()
{
    writeLog(Message102);
}

void ThreadPing::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    const auto messageName = message->name();

    writeLog(Message120, messageName.c_str());

    if (MESSAGE_NAME_SEND_PING == messageName)
    {
        auto factory = dynamic_cast<PacketFactoryAsciiLines*>(packetFactory());
        if (factory)
        {
            std::string sendString = "Ping " + std::to_string(DateTimeUtils::getTickCount());
            auto packet = factory->buildPacket(sendString.c_str());
            if (packet)
            {
                sendPacket(packet);
            }
        }
    }
}

void ThreadPing::onConnecting(const HandlerBase &handler)
{
    writeLog(Message110, handler.deviceName().c_str());
}

void ThreadPing::onConnected(const HandlerBase &handler)
{
    writeLog(Message111, handler.deviceName().c_str());

    startThreadTimer(MESSAGE_NAME_SEND_PING, 5000, true);
}

void ThreadPing::onDisconnected(const HandlerBase &handler)
{
    writeLog(Message113, handler.deviceName().c_str());

    stopThreadTimer(MESSAGE_NAME_SEND_PING);
}

void ThreadPing::onReadData(const HandlerBase &handler,
                            const void *,
                            const int &count)
{
    writeLog(Message114, count, handler.deviceName().c_str());
}

void ThreadPing::onWriteData(const HandlerBase &handler,
                             const void *,
                             const int &count)
{
    writeLog(Message115, count, handler.deviceName().c_str());
}

void ThreadPing::onError(HandlerBase &handler,
                         const int errorCode,
                         const std::string &)
{
    writeLog(Message112, handler.deviceName().c_str(), errorString(errorCode).c_str());
}

bool ThreadPing::onPacketReceived(const PacketBase::Ptr &packet)
{
    if (!packet)
        return false;

    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);
    writeLog(Message116, packetAscii->line().c_str(), handler()->deviceName().c_str());

    return true;
}

bool ThreadPing::onPacketSent(const PacketBase::Ptr &packet)
{
    if (!packet)
        return false;

    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);
    writeLog(Message117, packetAscii->line().c_str(), handler()->deviceName().c_str());

    return true;
}
