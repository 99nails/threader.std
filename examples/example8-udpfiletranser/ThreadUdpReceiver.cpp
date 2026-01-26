#include "ThreadUdpReceiver.h"

#include "Example8Common.h"

#include <HandlerUdpSocket.h>

ThreadUdpReceiver::ThreadUdpReceiver(MessageConsumer *parent)
    : ThreadHandler(parent,
                    new HandlerUdpSocket(this, "0.0.0.0", 63814),
                    10000)
{
}


void ThreadUdpReceiver::onError(HandlerBase *sender,
                                const int errorCode,
                                const std::string &errorDescription)
{
    if (!handler())
        return;

    writeLog(Message1013, handler()->deviceName().c_str(), errorCode, errorString(errorCode).c_str());
}

void ThreadUdpReceiver::onThreadStarted()
{
    if (!handler())
        return;

    writeLog(Message1000, handler()->deviceName().c_str());
}

void ThreadUdpReceiver::onThreadFinishing()
{
    if (!handler())
        return;

    writeLog(Message1001, handler()->deviceName().c_str());
}

void ThreadUdpReceiver::onThreadFinished()
{
    if (!handler())
        return;

    writeLog(Message1002, handler()->deviceName().c_str());
}

void ThreadUdpReceiver::onIdle()
{
}

void ThreadUdpReceiver::processMessage(const MessageBase::Ptr &)
{
}

void ThreadUdpReceiver::onConnecting()
{
    if (!handler())
        return;

    writeLog(Message1010, handler()->deviceName().c_str());
}

void ThreadUdpReceiver::onConnected()
{
    if (!handler())
        return;

    writeLog(Message1011, handler()->deviceName().c_str());
}

void ThreadUdpReceiver::onDisconnected()
{
    if (!handler())
        return;

    writeLog(Message1012, handler()->deviceName().c_str());
}

void ThreadUdpReceiver::onReadData(const void *, const int &)
{
}

void ThreadUdpReceiver::onWriteData(const void *, const int &)
{
}

bool ThreadUdpReceiver::onPacketReceived(const PacketBase::Ptr &)
{
    return true;
}

bool ThreadUdpReceiver::onPacketSent(const PacketBase::Ptr &)
{
    return true;
}
