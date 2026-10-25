#include "ThreadSslServerConnection.h"

#include "Example13Common.h"

#include "HandlerTcpSocket.h"
#include "HandlerSslSocket.h"
#include "PacketFactoryAsciiLines.h"

ThreadSslServerConnection::ThreadSslServerConnection(MessageConsumer *parent,
                                                     const std::string &host,
                                                     const uint16_t port,
                                                     const Descriptor &descriptor,
                                                     const bool useSsl)
    : ThreadHandler(parent,
                    nullptr,
                    0,
                    new PacketFactoryAsciiLines(),
                    "Thread." + host + ":" + std::to_string(port))
{
    if (useSsl)
        setHandler(new HandlerSslSocket(this, host, port, descriptor));
    else
        setHandler(new HandlerTcpSocket(this, host, port, descriptor));

}

void ThreadSslServerConnection::onError(HandlerBase &sender,
                                        const int errorCode,
                                        const std::string &errorDescription)
{
    if (!handler())
        return;

    writeLog(Message220, handler()->deviceName().c_str(), errorCode, errorString(errorCode).c_str());
}

void ThreadSslServerConnection::onThreadStarted()
{
    if (!handler())
        return;

    writeLog(Message200, handler()->deviceName().c_str());
}

void ThreadSslServerConnection::onThreadFinishing()
{
    if (!handler())
        return;

    writeLog(Message201, handler()->deviceName().c_str());
}

void ThreadSslServerConnection::onThreadFinished()
{
    if (!handler())
        return;

    writeLog(Message202, handler()->deviceName().c_str());
}


void ThreadSslServerConnection::onConnected(const HandlerBase &handler)
{
    writeLog(Message210, handler.deviceName().c_str());
}

void ThreadSslServerConnection::onDisconnected(const HandlerBase &handler)
{
    writeLog(Message211, handler.deviceName().c_str());

    terminateThread();
}

bool ThreadSslServerConnection::onPacketReceived(const PacketBase::Ptr &packet)
{
    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);
    if (!packetAscii)
        return false;

    writeLog(Message214, packetAscii->line().c_str(), handler()->deviceName().c_str());

    if (parentThread())
        parentThread()->postMessage1<MessagePacket>(packetAscii);

    return true;
}

bool ThreadSslServerConnection::onPacketSent(const PacketBase::Ptr &packet)
{
    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);
    if (!packetAscii)
        return false;

    writeLog(Message215, packetAscii->line().c_str(), handler()->deviceName().c_str());
    return true;
}

void ThreadSslServerConnection::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    const auto messageName = message->name();
    if (messageName == MessagePacket::MESSAGE_NAME)
        doMessagePacket(std::dynamic_pointer_cast<MessagePacket>(message));
}

void ThreadSslServerConnection::doMessagePacket(const MessagePacket::Ptr &message)
{
    if (!message || !message->value())
        return;

    sendPacket(message->value());
}
