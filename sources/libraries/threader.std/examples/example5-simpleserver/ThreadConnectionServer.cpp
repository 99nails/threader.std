#include "ThreadConnectionServer.h"

#include "Example5Common.h"

#include "HandlerTcpSocket.h"

#include "PacketFactoryAsciiLines.h"

ThreadConnectionServer::ThreadConnectionServer(MessageConsumer *parent,
                                               const std::string &host,
                                               const uint16_t port,
                                               const Descriptor descriptor)
    : ThreadHandler(parent,
                    new HandlerTcpSocket(this, host, port, descriptor),
                    0,
                    new PacketFactoryAsciiLines(),
                    "Thread.Connection")
{
    if (handler())
        setThreadName(threadName() + "[" + handler()->deviceName() + "]");
}

void ThreadConnectionServer::onThreadStarted()
{
    writeLog(Message200, handler()->deviceName().c_str());
}

void ThreadConnectionServer::onThreadFinishing()
{
    writeLog(Message201, handler()->deviceName().c_str());
}

void ThreadConnectionServer::onThreadFinished()
{
    writeLog(Message202, handler()->deviceName().c_str());
}

void ThreadConnectionServer::onError(HandlerBase *sender,
                                     const int errorCode,
                                     const std::string &errorDescription)
{
    writeLog(Message220, handler()->deviceName().c_str(),
             errorCode, errorString(errorCode).c_str());
}

void ThreadConnectionServer::onConnected()
{
    writeLog(Message210, handler()->deviceName().c_str());
}

void ThreadConnectionServer::onDisconnected()
{
    writeLog(Message211, handler()->deviceName().c_str());
}

void ThreadConnectionServer::onReadData(const void *,
                                        const int &count)
{
    writeLog(Message212, count, handler()->deviceName().c_str());
}

void ThreadConnectionServer::onWriteData(const void *,
                                         const int &count)
{
    writeLog(Message213, count, handler()->deviceName().c_str());
}

bool ThreadConnectionServer::onPacketReceived(const PacketBase::Ptr &packet)
{
    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLines>(packet);
    if (!packetAscii)
        return false;

    writeLog(Message214,
             packetAscii->lines().front().c_str(),
             handler()->deviceName().c_str());

    return true;
}

bool ThreadConnectionServer::onPacketSent(const PacketBase::Ptr &packet)
{
    return true;
}
