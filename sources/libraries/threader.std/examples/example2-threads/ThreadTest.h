#pragma once

#include <ThreadBase.h>

using namespace Threader::Threads;;

class ThreadTest : public ThreadBase
{
public:
    explicit ThreadTest();

protected:
    void processMessage(const MessageBase::Ptr &message) override;

private:
    int _count;

    // MessageConsumer interface
protected:
    void onProcessMessagesStarted(const int count) override;
    void onProcessMessagesFinished() override;
};
