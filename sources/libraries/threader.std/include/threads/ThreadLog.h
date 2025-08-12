#pragma once

#include "Logger.h"
#include "ThreadBase.h"

using namespace Threader::Threads;

namespace Threader::Threads {

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Log"

MESSAGE_TEMPLATE(0, 0, MessageType::Message, "Запуск подсистемы прокотоколирования. Подробность [%d], хранение [%s]")
MESSAGE_TEMPLATE(1, 0, MessageType::Message, "Завершение подсистемы протоколирования...")
MESSAGE_TEMPLATE(2, 0, MessageType::Message, "Подсистема протоколирования завершена")

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Common"

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
