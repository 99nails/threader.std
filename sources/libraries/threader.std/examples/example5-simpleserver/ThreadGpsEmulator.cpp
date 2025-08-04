#include "ThreadGpsEmulator.h"

#include "Example5Common.h"

#include "HandlerSerialPort.h"
#include "PacketFactoryAsciiLines.h"

#include "StdStringUtils.h"

#include <time.h>
#include <ctime>
#include <chrono>

constexpr char MESSAGE_NAME_TIMER[] = "Message.Timer";

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;


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
    auto msecsSinceEpoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    std::time_t temp = msecsSinceEpoch / 1000;
    auto time = std::gmtime(&temp);

    auto timeString = format("$GNZDA,%.2d%.2d%.2d.000,%.2d,%.2d,%.4d,00,00*FF\r\n",
                             time->tm_hour, time->tm_min, time->tm_sec,
                             time->tm_mday, time->tm_mon, time->tm_year + 1900);

    auto packet = packetFactory()->buildPacket(timeString.c_str(), false);
    sendPacket(packet);
}

