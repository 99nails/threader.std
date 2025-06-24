#include "ThreadPing.h"

#include "Example4Common.h"

#include "HandlerSerialPort.h"
#include "PacketFactoryAsciiLines.h"

ThreadPing::ThreadPing()
    : ThreadHandler(nullptr,
                    new HandlerSerialPort(nullptr, "/dev/pts/1"),
                    1000,
                    new PacketFactoryAsciiLines(),
                    "Thread.Ping")
{
}

void ThreadPing::onThreadStarted()
{
    writeLog(Message100);

    startThreadTimer(MESSAGE_NAME_SEND_PING, 100, true);
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

    writeLog(Message101);
}

void ThreadPing::onConnecting()
{
    if (!handler())
        return;
    writeLog(Message110, handler()->deviceName().c_str());
}

void ThreadPing::onConnected()
{
    if (!handler())
        return;
    writeLog(Message111, handler()->deviceName().c_str());
}

void ThreadPing::onDisconnected()
{
    if (!handler())
        return;
    writeLog(Message113, handler()->deviceName().c_str());
}

void ThreadPing::onReadData(const void *, const int &count)
{
    if (!handler())
        return;
    writeLog(Message114, count, handler()->deviceName().c_str());
}

void ThreadPing::onWriteData(const void *, const int &count)
{
    if (!handler())
        return;
    writeLog(Message115, count, handler()->deviceName().c_str());
}

void ThreadPing::onError(HandlerBase *handler,
                         const int errorCode,
                         const std::string &errorDescription)
{
    if (!handler)
        return;
    writeLog(Message112,
             handler->deviceName().c_str(),
             errorString(errorCode).c_str());
    (void)errorDescription;
}

bool ThreadPing::onPacketReceived(const PacketBase::Ptr &packet)
{
    if (!packet)
        return false;

    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLines>(packet);
    writeLog(Message116, packetAscii->lines().front().c_str(), handler()->deviceName().c_str());

    return true;
}

bool ThreadPing::onPacketSent(const PacketBase::Ptr &packet)
{
    if (!packet)
        return false;

    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLines>(packet);
    writeLog(Message117, packetAscii->lines().front().c_str(), handler()->deviceName().c_str());

    return true;
}
