#include "ThreadGps.h"

#include "Example5Common.h"

#include "HandlerSerialPort.h"
#include "PacketFactoryAsciiLines.h"

#include "StdStringUtils.h"

#include <time.h>
#include <ctime>
#include <chrono>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;


ThreadGps::ThreadGps(const SettingsGps::Ptr &settings,
                     MessageConsumer *parent)
    : ThreadHandler(parent,
                    new HandlerSerialPort(nullptr, (settings) ? settings->port() : DEFAULT_GPS_PORT),
                    1000,
                    new PacketFactoryAsciiLines(),
                    "Thread.Gps")
    , _settings(settings)
{
}

void ThreadGps::onThreadStarted()
{
    if (!handler())
        return;
    writeLog(Message400, handler()->deviceName().c_str());
}

void ThreadGps::onThreadFinishing()
{
    if (!handler())
        return;
    writeLog(Message401, handler()->deviceName().c_str());
}

void ThreadGps::onThreadFinished()
{
    if (!handler())
        return;
    writeLog(Message402, handler()->deviceName().c_str());
}

void ThreadGps::onConnecting()
{
    if (!handler())
        return;
    writeLog(Message410, handler()->deviceName().c_str());
}

void ThreadGps::onConnected()
{
    if (!handler())
        return;
    writeLog(Message411, handler()->deviceName().c_str());
}

void ThreadGps::onDisconnected()
{
    if (!handler())
        return;
    writeLog(Message413, handler()->deviceName().c_str());
}

void ThreadGps::onReadData(const void *, const int &count)
{
    if (!handler())
        return;
    writeLog(Message414, count, handler()->deviceName().c_str());
}

void ThreadGps::onWriteData(const void *, const int &count)
{
    if (!handler())
        return;
    writeLog(Message415, count, handler()->deviceName().c_str());
}

void ThreadGps::onError(HandlerBase *handler,
                                const int errorCode,
                                const std::string &errorDescription)
{
    if (!handler)
        return;
    writeLog(Message412, handler->deviceName().c_str(), errorString(errorCode).c_str());
}

bool ThreadGps::onPacketReceived(const PacketBase::Ptr &packet)
{
    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);

    writeLog(Message420, packetAscii->line().c_str());

    auto line = packetAscii->line();

    if (!StdStringUtils::startWith(line, GPS_PREFIX))
        return false;

    line.erase(0, 1);

    auto words = StdStringUtils::split(line, GPS_DELIMITER, true);
    if (words.size() < 1)
        return false;

    if (GPS_PREFIX_GNZDA == words.front())
        doGpsCommandGNZDA(words);

    return true;
}

bool ThreadGps::onPacketSent(const PacketBase::Ptr &packet)
{
    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);

    writeLog(Message421, packetAscii->line().c_str());

    return true;
}

void ThreadGps::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    auto messageName = message->name();
}

void ThreadGps::doGpsCommandGNZDA(const StdStringVector &words)
{
    auto timeString = words[1].substr(0, 6);
    timeString = timeString.insert(4, 1, ':').insert(2, 1, ':');
    auto dateTimeString = words[4] + "-" + words[3] + "-" + words[2] + " " +
            timeString;

    auto dateTime = DateTimeUtils::StdStringToMsecsSinceEpoch(dateTimeString,
                                                              "%Y-%m-%d %H:%M:%S");
    if (dateTime > 0)
    {
        writeLog(Message430, dateTimeString.c_str());
        if (parentThread())
            parentThread()->postMessage(std::make_shared<MessageString>(MESSAGE_NAME_DATETIME,
                                                                        dateTimeString));
    }
}

