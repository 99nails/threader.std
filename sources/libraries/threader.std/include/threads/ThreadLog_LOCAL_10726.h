#pragma once

#include "Logger.h"
#include "MessageBase.h"
#include "MessageWriteToFile.h"
#include "ThreadBase.h"

namespace Threader::Threads
{

class ThreadLog : public ThreadBase
{
public:
    explicit ThreadLog(const std::string &pathToBin,
                       MessageConsumer *consumer = nullptr,
                       const bool console = false,
                       const int level = 9,
                       const bool clearHistory = true,
                       const int historyDepth = 30,
                       const bool pack = true,
                       const int packDepth = 2);
    virtual ~ThreadLog();

    static void setLevel(const int level);

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
    void doMessageLogLevel(const MessageInt::Ptr &message);
private:
    static ThreadLog *_instance;
    Logger *_logger;
    MessageConsumer *_consumer;
    ThreadBase *_threadLogCleaner;
};

} // namespace Threader::Threads
