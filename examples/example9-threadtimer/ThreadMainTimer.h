#pragma once

#include "ThreadMain.h"

using namespace Threader::Threads;

class ThreadMainTimer : public ThreadMainWithouSettings
{
public:
    static constexpr char TIMER_NAME_LONG[] = "Timer.Long";
    static constexpr char TIMER_NAME_SHORT[] = "Timer.Short";
    static constexpr char TIMER_NAME_ONE_SHOT[] = "Timer.One.Shot";

    static constexpr uint32_t TIMEOUT_LONG = 5000;
    static constexpr uint32_t TIMEOUT_SHORT = 500;
    static constexpr uint32_t TIMEOUT_ONE_SHOT = 13000;
public:
    explicit ThreadMainTimer(const std::string &applicationBinaryName,
                             const std::string &applicationFriendlyName);

protected:
    void onThreadStarted() override;

    void processMessage(const MessageBase::Ptr &message) override;

private:
    void startTimerLong();
    void doTimerLong();

    void startTimerShort();
    void stopTimerShort();
    void doTimerShort();

    void startTimerOneShot();
    void doTimerOneShot();

private:
    bool _timerLongSwitch;
};

