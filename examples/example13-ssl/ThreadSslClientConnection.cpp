#include "ThreadSslClientConnection.h"

#include "Example13Common.h"
#include "HandlerTcpSocket.h"
#include "HandlerSslSocket.h"

ThreadSslClientConnection::ThreadSslClientConnection(MessageConsumer *parent,
                                                     const std::string &host,
                                                     const uint16_t port,
                                                     const bool useSsl)
    : ThreadHandler(parent,
                    nullptr,
                    10000,
                    new PacketFactoryAsciiLines(),
                    "Thread." + host + ":" + std::to_string(port))
    , _useSsl(useSsl)
{
    if (useSsl)
        setHandler(new HandlerSslSocket(this, host, port, INVALID_DESCRIPTOR));
    else
        setHandler(new HandlerTcpSocket(this, host, port, INVALID_DESCRIPTOR));

    _factory = dynamic_cast<PacketFactoryAsciiLines*>(packetFactory());

    setTimeout(20000);
}

void ThreadSslClientConnection::onError(HandlerBase &sender,
                                        const int errorCode,
                                        const std::string &errorDescription)
{
    if (!handler())
        return;

    writeLog(Message320, handler()->deviceName().c_str(), errorCode, errorString(errorCode).c_str());
}

void ThreadSslClientConnection::onThreadStarted()
{
    if (!handler())
        return;

    writeLog(Message300, handler()->deviceName().c_str());
}

void ThreadSslClientConnection::onThreadFinishing()
{
    if (!handler())
        return;

    writeLog(Message301, handler()->deviceName().c_str());
}

void ThreadSslClientConnection::onThreadFinished()
{
    if (!handler())
        return;

    writeLog(Message302, handler()->deviceName().c_str());
}

void ThreadSslClientConnection::onIdle()
{
    if (!_factory)
        return;

    if (!_useSsl)
        return;

    auto line = std::string((_useSsl) ? "SSL" : "Open") + " packet " + std::to_string(++_count);
    auto packet = _factory->buildPacket(line.c_str());
    sendPacket(packet);
}

void ThreadSslClientConnection::onConnecting(const HandlerBase &handler)
{
    writeLog(Message309, handler.deviceName().c_str());
}

void ThreadSslClientConnection::onConnected(const HandlerBase &handler)
{
    writeLog(Message310, handler.deviceName().c_str());
    _count = 0;
}

void ThreadSslClientConnection::onDisconnected(const HandlerBase &handler)
{
    writeLog(Message311, handler.deviceName().c_str());
}

bool ThreadSslClientConnection::onPacketReceived(const PacketBase::Ptr &packet)
{
    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);
    if (!packetAscii)
        return false;

    writeLog(Message314, packetAscii->line().c_str(), handler()->deviceName().c_str());
    return true;
}

bool ThreadSslClientConnection::onPacketSent(const PacketBase::Ptr &packet)
{
    auto packetAscii = std::dynamic_pointer_cast<PacketAsciiLine>(packet);
    if (!packetAscii)
        return false;

    writeLog(Message315, packetAscii->line().c_str(), handler()->deviceName().c_str());
    return true;
}
