#pragma once

#include "Example13Common.h"

#include "ThreadMain.h"

using namespace Threader::Threads;

class ThreadMainSslServer : public ThreadMainWithouSettings
{
public:
    explicit ThreadMainSslServer(const std::string &applicationBinaryName,
                                 const std::string &applicationFriendlyName);

protected:
    void startChildThreads() override;

    void processMessage(const MessageBase::Ptr &message) override;

private:
    void doMessageThread(const MessageThread::Ptr &message);
    void doMessagePacket(const MessagePacket::Ptr &message);

private:
    ThreadBase *_threadListen;

};
