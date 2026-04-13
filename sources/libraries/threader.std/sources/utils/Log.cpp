#include "Log.h"

#include "ThreadLog.h"
#include "StdStringUtils.h"

#include <cstdarg>

namespace Threader::Utils {

bool startLogSubsystem(const std::string &pathToBin,
                       MessageConsumer *consumer,
                       bool console,
                       int level)
{
    if (ThreadBase::logThread())
        return true;

    auto thread = new ThreadLog(pathToBin, consumer, console, level);
    thread->startThread();

    return true;
}

void stopLogSubsystem()
{
    auto logThread = ThreadBase::logThread();
    if (!logThread)
        return;

    logThread->postTerminateEvent();
    while (!logThread->isThreadFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }
}

void writeLog(const MessageLogTemplate messageTemplate, ...)
{
    auto logThread = ThreadBase::logThread();
    if (!logThread)
        return;

    va_list list;
    va_start(list, messageTemplate);
    auto messageText = format(messageTemplate.text.c_str(), list);
    va_end(list);

    auto message = std::make_shared<MessageLog>(messageText,
                                                messageTemplate.number,
                                                messageTemplate.level,
                                                messageTemplate.subSystem,
                                                messageTemplate.messageType);
    logThread->postMessage(message);
}

}
