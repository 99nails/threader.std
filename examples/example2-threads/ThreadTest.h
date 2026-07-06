#pragma once

#include <ThreadBase.h>

using namespace Threader::Threads;

DECLARE_MESSAGE_NAMED(NamedString, "Message.Named.String", std::string)

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
    void onProcessMessagesStarted(const int32_t count) override;
    void onProcessMessagesFinished() override;
};
