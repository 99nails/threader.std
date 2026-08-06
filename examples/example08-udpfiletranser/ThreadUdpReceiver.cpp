#include "ThreadUdpReceiver.h"

#include "Example08Common.h"

#include <HandlerUdpSocket.h>
#include <SocketUtils.h>

using namespace Threader::Utils;

ThreadUdpReceiver::ThreadUdpReceiver(MessageConsumer *parent,
                                     const std::string bindIpAddress)
    : ThreadConnections(parent, {})
    , _bindIpAddress(bindIpAddress)
    , _connectionIn(nullptr)
    , _connectionOut(nullptr)
{

    _connectionIn = registerConnectionHandler(new HandlerUdpSocket(nullptr,
                                                                   _bindIpAddress,
                                                                   5000,
                                                                   UdpSocketType::Receiver,
                                                                   _bindIpAddress));

    _connectionOut = registerConnectionHandler(new HandlerUdpSocket(nullptr,
                                                                    "172.16.30.178",
                                                                    5000,
                                                                    UdpSocketType::Sender,
                                                                    _bindIpAddress));

    setTimeout(100);
}


void ThreadUdpReceiver::onError(const ConnectionBase &connection,
                                const int &errorCode,
                                const std::string &errorDescription)
{
    if (!connection.handler())
        return;

    auto handlerUdp = dynamic_cast<HandlerUdpSocket*>(connection.handler());
    bool isReceiver = handlerUdp->socketType() == UdpSocketType::Receiver;

    writeLog((isReceiver) ? Message1013 : Message1023,
             handlerUdp->deviceName().c_str(), errorCode, errorString(errorCode).c_str());
}

void ThreadUdpReceiver::onThreadStarted()
{
    if (!_connectionIn)
        return;

    writeLog(Message1000);
}

void ThreadUdpReceiver::onThreadFinishing()
{
    if (!_connectionIn)
        return;

    writeLog(Message1001);
}

void ThreadUdpReceiver::onThreadFinished()
{
    if (!_connectionIn)
        return;

    writeLog(Message1002);
}

void ThreadUdpReceiver::onIdle()
{
    if (!_connectionIn || !_connectionIn->handler() || !_connectionIn->handler()->isConnected())
        return;

//    auto handler = dynamic_cast<HandlerUdpSocket*>(_connectionIn->handler());
//    handler->writeTo("172.16.30.178", "Test message", 11);
//    _connectionIn->sendString(std::string("Test string"));
}

void ThreadUdpReceiver::processMessage(const MessageBase::Ptr &)
{
}

void ThreadUdpReceiver::onConnecting(const ConnectionBase &connection)
{
    if (!connection.handler())
        return;

    auto handlerUdp = dynamic_cast<HandlerUdpSocket*>(connection.handler());
    bool isReceiver = handlerUdp->socketType() == UdpSocketType::Receiver;

    writeLog((isReceiver) ? Message1010 : Message1020,
             handlerUdp->deviceName().c_str());
}

void ThreadUdpReceiver::onConnected(const ConnectionBase &connection)
{
    if (!connection.handler())
        return;

    auto handlerUdp = dynamic_cast<HandlerUdpSocket*>(connection.handler());
    bool isReceiver = handlerUdp->socketType() == UdpSocketType::Receiver;

    writeLog((isReceiver) ? Message1011 : Message1021,
             handlerUdp->deviceName().c_str());

}

void ThreadUdpReceiver::onDisconnected(const ConnectionBase &connection)
{
    if (!connection.handler())
        return;

    auto handlerUdp = dynamic_cast<HandlerUdpSocket*>(connection.handler());
    bool isReceiver = handlerUdp->socketType() == UdpSocketType::Receiver;

    writeLog((isReceiver) ? Message1012 : Message1022,
             handlerUdp->deviceName().c_str());
}

void ThreadUdpReceiver::onReadData(const ConnectionBase &connection,
                                   const char *,
                                   const int &size)
{
    if (!connection.handler())
        return;

    auto hadlerUdp = dynamic_cast<HandlerUdpSocket*>(connection.handler());
    auto senderIpAddress = SocketUtils::sockAddrToString(hadlerUdp->readAddress());

    writeLog(Message1030, size, senderIpAddress.c_str());
//    writeLog(Message1030, size, hadlerUdp->deviceName().c_str());
}

void ThreadUdpReceiver::onWriteData(const ConnectionBase &connection,
                                    const char *,
                                    const int &size)
{
    if (!connection.handler())
        return;

    writeLog(Message1031, size, connection.handler()->deviceName().c_str());
}

void ThreadUdpReceiver::onPacketReceived(const ConnectionBase &connection,
                                         const PacketBase::Ptr &)
{
}

void ThreadUdpReceiver::onPacketSent(const ConnectionBase &connection,
                                     const PacketBase::Ptr &)
{
}
