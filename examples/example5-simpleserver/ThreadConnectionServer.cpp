#include "ThreadConnectionServer.h"

#include "Example5Common.h"

#include "HandlerTcpSocket.h"

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
    _factory = dynamic_cast<PacketFactoryAsciiLines*>(packetFactory());
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

void ThreadConnectionServer::onConnected(const HandlerBase *handler)
{
    if (handler)
        return;

    writeLog(Message210, handler->deviceName().c_str());
}

void ThreadConnectionServer::onDisconnected(const HandlerBase *handler)
{
    if (!handler)
        return;

    writeLog(Message211, handler->deviceName().c_str());

    terminateThread();
}

void ThreadConnectionServer::onReadData(const HandlerBase *handler, const void *,
                                        const int &count)
{
    writeLog(Message212, count, handler->deviceName().c_str());
}

void ThreadConnectionServer::onWriteData(const HandlerBase *handler,
                                         const void *,
                                         const int &count)
{
    writeLog(Message213, count, handler->deviceName().c_str());
}

bool ThreadConnectionServer::onPacketReceived(const PacketBase::Ptr &packet)
{
    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);
    if (!packetAscii)
        return false;

    writeLog(Message214,
             packetAscii->line().c_str(),
             handler()->deviceName().c_str());

    return true;
}

bool ThreadConnectionServer::onPacketSent(const PacketBase::Ptr &packet)
{
    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);
    if (!packetAscii)
        return false;

    writeLog(Message215,
             packetAscii->line().c_str(),
             handler()->deviceName().c_str());
    return true;
}

void ThreadConnectionServer::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    auto messageName = message->name();
    if (MESSAGE_NAME_DATETIME == messageName)
        doMessageDateTime(std::dynamic_pointer_cast<MessageString>(message));
}

void ThreadConnectionServer::doMessageDateTime(const MessageString::Ptr &message)
{
    if (!message)
        return;

    if (_factory)
    {
        auto packet = _factory->buildPacket(message->value().c_str());
        if (packet)
            sendPacket(packet);
    }
}
