#include "ThreadConnectionServer.h"

#include "Example5Common.h"

#include "HandlerTcpSocket.h"

ThreadConnectionServer::ThreadConnectionServer(MessageConsumer *parent,
                                               const std::string &host,
                                               const uint16_t port,
                                               const Descriptor descriptor)
    : ThreadHandler(parent,
                    nullptr,
//                    new HandlerTcpSocket(this, host, port, descriptor),
                    0,
                    nullptr,
                    "Thread.Connection")
{
    setHandler(new HandlerTcpSocket(this, host, port, descriptor));
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

void ThreadConnectionServer::onError(HandlerBase *sender, const int errorCode, const std::string &errorDescription)
{
}

bool ThreadConnectionServer::onPacketReceived(const PacketBase::Ptr &packet)
{
    return true;
}

bool ThreadConnectionServer::onPacketSent(const PacketBase::Ptr &packet)
{
    return true;
}


