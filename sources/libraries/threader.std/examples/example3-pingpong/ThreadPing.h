#pragma once

#include <ThreadBase.h>

using namespace Threader::Threads;

class ThreadPing : public ThreadBase
{
public:
    explicit ThreadPing();

    void setThreadPong(ThreadBase * threadPong);

protected:
    void onThreadStarted() override;
    void onThreadFinished() override;
    void processMessage(const MessageBase::Ptr &message) override;

private:
    ThreadBase *_threadPong;
};
