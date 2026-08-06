#include "ThreadConnectionClient.h"

#include "Example05Common.h"

#include <HandlerTcpSocket.h>
#include <PacketFactoryAsciiLines.h>

#include <cmath>

ThreadConnectionClient::ThreadConnectionClient(MessageConsumer *parent,
                                               const SettingsTimeServer::Ptr &settings)
    : ThreadHandler(parent,
                    new HandlerTcpSocket(this, settings->host(), settings->port(), INVALID_DESCRIPTOR),
                    settings->reconnectTimeout() * 1000,
                    new PacketFactoryAsciiLines(),
                    "Thread.Connection")
    , _settings(settings)
{
    if (handler())
        setThreadName(threadName() + "[" + handler()->deviceName() + "]");

    time_t t = time(NULL);
    struct tm lt = {0};
    localtime_r(&t, &lt);
    _gmtOffset = lt.tm_gmtoff;
}


void ThreadConnectionClient::onError(HandlerBase &sender,
                                     const int errorCode,
                                     const std::string &errorDescription)
{
    writeLog(Message520, handler()->deviceName().c_str(),
             errorCode, errorString(errorCode).c_str());
}


void ThreadConnectionClient::onThreadStarted()
{
    writeLog(Message500, handler()->deviceName().c_str());
}

void ThreadConnectionClient::onThreadFinishing()
{
    writeLog(Message501, handler()->deviceName().c_str());
}

void ThreadConnectionClient::onThreadFinished()
{
    writeLog(Message502, handler()->deviceName().c_str());
}


void ThreadConnectionClient::onConnecting(const HandlerBase &handler)
{
    writeLog(Message509, handler.deviceName().c_str());
}

void ThreadConnectionClient::onConnected(const HandlerBase &handler)
{
    writeLog(Message510, handler.deviceName().c_str());
}

void ThreadConnectionClient::onDisconnected(const HandlerBase &handler)
{
    writeLog(Message511, handler.deviceName().c_str());
}

void ThreadConnectionClient::onReadData(const HandlerBase &handler,
                                        const void *,
                                        const int &count)
{
    writeLog(Message512, count, handler.deviceName().c_str());
}

void ThreadConnectionClient::onWriteData(const HandlerBase &handler,
                                         const void *,
                                         const int &count)
{
    writeLog(Message512, count, handler.deviceName().c_str());
}

bool ThreadConnectionClient::onPacketReceived(const PacketBase::Ptr &packet)
{
    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);
    if (!packetAscii)
        return false;

    writeLog(Message514,
             packetAscii->line().c_str(),
             handler()->deviceName().c_str());

    auto dateTime = DateTimeUtils::StdStringToMsecsSinceEpoch(packetAscii->line(),
                                                              "%Y-%m-%d %H:%M:%S");
    if (dateTime == 0)
        return false;

    writeLog(Message530, packetAscii->line().c_str());

    auto dateTimeNow = DateTimeUtils::msecsSinceEpochLocal() - _gmtOffset * 1000;

    int64_t delta = dateTimeNow - dateTime;
    if (std::abs(delta) > 5000)
    {

        tm dateTimeTm;
        strptime(packetAscii->line().c_str(),
                 "%Y-%m-%d %H:%M:%S",
                 &dateTimeTm);
        if (dateTimeTm.tm_year > 0)
        {
            dateTimeTm.tm_mon++;
            dateTimeTm.tm_year += 1900;

            DateTimeUtils::setCurrentDateTime(dateTime + _gmtOffset * 1000);
        }
    }

    return true;
}

bool ThreadConnectionClient::onPacketSent(const PacketBase::Ptr &packet)
{
    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);
    if (!packetAscii)
        return false;

    writeLog(Message515,
             packetAscii->line().c_str(),
             handler()->deviceName().c_str());

    return true;
}
