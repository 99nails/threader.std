#pragma once

#include "Example5Common.h"

#include "ThreadBase.h"

using namespace Threader::Threads;

class ThreadMainExample5 : public ThreadBase
{
public:
    explicit ThreadMainExample5();

protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;
    void startChildThreads() override;

    void processMessage(const MessageBase::Ptr &message) override;

private:
    ThreadBase *_threadListen;

    void doMessageThread(const MessageThread::Ptr &message);
};

