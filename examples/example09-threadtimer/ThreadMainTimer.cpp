#include "ThreadMainTimer.h"

#include "Logger.h"

MESSAGE_TEMPLATE(1000, 9, MessageType::Message,
                 "Таймер сообщений остановлен и запустится через [%d] мсек.")

MESSAGE_TEMPLATE(1001, 9, MessageType::Message,
                 "Таймер сообщений запущен и остановится через [%d] мсек.")

MESSAGE_TEMPLATE(1002, 9, MessageType::Message,
                 "Таймер сообщений сработал")

MESSAGE_TEMPLATE(1003, 9, MessageType::Message,
                 "Одиночный таймер сработает через [%d] мсек.")

MESSAGE_TEMPLATE(1004, 9, MessageType::Message,
                 "Одиночный таймер сработал")

ThreadMainTimer::ThreadMainTimer(const std::string &applicationBinaryName,
                                 const std::string &applicationFriendlyName)
    : ThreadMainWithouSettings(nullptr,
                               applicationBinaryName,
                               applicationFriendlyName)
    , _timerLongSwitch(false)
{
}

void ThreadMainTimer::onThreadStarted()
{
    startTimerLong();
    startTimerOneShot();
}

void ThreadMainTimer::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    const auto messageName = message->name();
    if (messageName == TIMER_NAME_LONG)
        doTimerLong();
    else if (messageName == TIMER_NAME_SHORT)
        doTimerShort();
    else if (messageName == TIMER_NAME_ONE_SHOT)
        doTimerOneShot();
}

void ThreadMainTimer::startTimerLong()
{
    _timerLongSwitch = false;
    writeLog(Message1000, TIMEOUT_LONG);
    startThreadTimer(TIMER_NAME_LONG, TIMEOUT_LONG, true);
}

void ThreadMainTimer::doTimerLong()
{
    _timerLongSwitch = !_timerLongSwitch;

    if (_timerLongSwitch)
    {
        writeLog(Message1001, TIMEOUT_LONG);
        startTimerShort();
    }
    else
    {
        stopTimerShort();
        writeLog(Message1000, TIMEOUT_LONG);
    }
}

void ThreadMainTimer::startTimerShort()
{
    startThreadTimer(TIMER_NAME_SHORT, TIMEOUT_SHORT, true);
}

void ThreadMainTimer::stopTimerShort()
{
    stopThreadTimer(TIMER_NAME_SHORT);
}

void ThreadMainTimer::doTimerShort()
{
    writeLog(Message1002);
}

void ThreadMainTimer::startTimerOneShot()
{
    startThreadTimer(TIMER_NAME_ONE_SHOT, TIMEOUT_ONE_SHOT, false);
    writeLog(Message1003, TIMEOUT_ONE_SHOT);
}

void ThreadMainTimer::doTimerOneShot()
{
    writeLog(Message1004);
}
