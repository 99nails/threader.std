#pragma once

#include "Logger.h"
#include "ThreadBase.h"

using namespace Threader::Threads;

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Log"

MESSAGE_TEMPLATE(0, 0, MessageType::Message, "Starting log subsystem...")
MESSAGE_TEMPLATE(1, 0, MessageType::Message, "Stopping log subsystem...")
MESSAGE_TEMPLATE(2, 0, MessageType::Message, "Log subsystem stopped")

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Common"

namespace Threader::Threads {

class ThreadLog : public ThreadBase
{
public:
    explicit ThreadLog(const std::string &pathToBin);
    virtual ~ThreadLog();

protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;

    void processMessage(const MessageBase::Ptr &message) override;

private:
    static ThreadLog *_instance;
    Logger *_logger;
};

} // namespace Threader::Threads
