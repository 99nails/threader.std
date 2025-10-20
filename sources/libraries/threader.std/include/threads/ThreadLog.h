#pragma once

#include "Logger.h"
#include "LogMessagesTemplates.h"
#include "MessageWriteToFile.h"
#include "ThreadBase.h"

using namespace Threader::Threads;

namespace Threader::Threads {


bool startLogSubsystem(const std::string &pathToBin,
                       MessageConsumer *consumer = nullptr,
                       int level = 9);

void stopLogSubsystem();

class ThreadLog : public ThreadBase
{
public:
    explicit ThreadLog(const std::string &pathToBin,
                       MessageConsumer *consumer = nullptr,
                       int level = 9);
    virtual ~ThreadLog();

    void flush();
protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;
    void onIdle() override;

    void processMessage(const MessageBase::Ptr &message) override;

private:
    void doMessageLog(const MessageLog::Ptr &message);
    void doMessageWriteToFile(const MessageWriteToFile::Ptr &message);

private:
    static ThreadLog *_instance;
    Logger *_logger;
    MessageConsumer *_consumer;
};

} // namespace Threader::Threads
