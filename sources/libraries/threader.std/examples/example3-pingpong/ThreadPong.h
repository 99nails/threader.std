#pragma once

#include <ThreadBase.h>

using namespace Threader::Threads;

class ThreadPong : public ThreadBase
{
public:
    explicit ThreadPong();

    void setThreadPing(ThreadBase * threadPing);

protected:
    void onThreadStarted() override;
    void onThreadFinished() override;

    void processMessage(const MessageBase::Ptr &message) override;

private:
    ThreadBase *_threadPing;
};
