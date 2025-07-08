#include "ThreadGpsEmulator.h"

#include "Example5Common.h"

#include "HandlerSerialPort.h"
#include "PacketFactoryAsciiLines.h"

constexpr char MESSAGE_NAME_TIMER[] = "Message.Timer";

ThreadGpsEmulator::ThreadGpsEmulator(SettingsGpsEmulator *settings,
                                     MessageConsumer *parent)
    : ThreadHandler(parent,
                    new HandlerSerialPort(nullptr, settings->portName()),
                    1000,
                    new PacketFactoryAsciiLines(),
                    "Thread.Gps.Emulator")
    , _settings(settings)
{
}

void ThreadGpsEmulator::onThreadStarted()
{
    if (!handler())
        return;
    writeLog(Message300, handler()->deviceName().c_str());
}

void ThreadGpsEmulator::onThreadFinishing()
{
    if (!handler())
        return;
    writeLog(Message301, handler()->deviceName().c_str());
}

void ThreadGpsEmulator::onThreadFinished()
{
    if (!handler())
        return;
    writeLog(Message302, handler()->deviceName().c_str());
}

void ThreadGpsEmulator::onConnecting()
{
    if (!handler())
        return;
    writeLog(Message310, handler()->deviceName().c_str());
}

void ThreadGpsEmulator::onConnected()
{
    if (!handler())
        return;
    writeLog(Message311, handler()->deviceName().c_str());

    startThreadTimer(MESSAGE_NAME_TIMER, 2000, true);
}

void ThreadGpsEmulator::onDisconnected()
{
    if (!handler())
        return;
    writeLog(Message313, handler()->deviceName().c_str());

    stopThreadTimer(MESSAGE_NAME_TIMER);
}

void ThreadGpsEmulator::onReadData(const void *, const int &count)
{
    if (!handler())
        return;
    writeLog(Message314, count, handler()->deviceName().c_str());
}

void ThreadGpsEmulator::onWriteData(const void *, const int &count)
{
    if (!handler())
        return;
    writeLog(Message315, count, handler()->deviceName().c_str());
}

void ThreadGpsEmulator::onError(HandlerBase *handler,
                                const int errorCode,
                                const std::string &errorDescription)
{
    if (!handler)
        return;
    writeLog(Message312, handler->deviceName().c_str(), errorString(errorCode).c_str());
}

bool ThreadGpsEmulator::onPacketReceived(const PacketBase::Ptr &packet)
{
    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLines>(packet);

    if (!packetAscii || packetAscii->lines().size() < 1)
        return false;

    writeLog(Message320, packetAscii->lines().front().c_str());

    return true;
}

bool ThreadGpsEmulator::onPacketSent(const PacketBase::Ptr &packet)
{
    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLines>(packet);

    if (!packetAscii || packetAscii->lines().size() < 1)
        return false;

    writeLog(Message321, packetAscii->lines().front().c_str());

    return true;
}

void ThreadGpsEmulator::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    auto messageName = message->name();

    if (MESSAGE_NAME_TIMER == messageName)
        sendGpsData();
}

void ThreadGpsEmulator::sendGpsData()
{
    auto packet = packetFactory()->buildPacket("$RMC\r\n", false);
    sendPacket(packet);
}

