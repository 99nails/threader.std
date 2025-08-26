#pragma once

#include "Logger.h"
#include "LogMessagesTemplates.h"
#include "ThreadBase.h"

using namespace Threader::Threads;

namespace Threader::Threads {


bool startLogSubsystem(const std::string &pathToBin,
                       MessageConsumer *consumer = nullptr);

void stopLogSubsystem();

class ThreadLog : public ThreadBase
{
public:
    explicit ThreadLog(const std::string &pathToBin,
                       MessageConsumer *consumer = nullptr);
    virtual ~ThreadLog();

    void flush();
protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;
    void onIdle() override;

    void processMessage(const MessageBase::Ptr &message) override;

private:
    static ThreadLog *_instance;
    Logger *_logger;
    MessageConsumer *_consumer;
};

} // namespace Threader::Threads
