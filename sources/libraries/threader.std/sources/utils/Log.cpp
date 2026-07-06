#include "Log.h"

#include "ThreadLog.h"
#include "ThreadLogCleaner.h"
#include "StdStringUtils.h"

#include <cstdarg>

namespace Threader::Utils {

bool startLogSubsystem(const std::string &pathToBin,
                       MessageConsumer *consumer,
                       const bool console,
                       const int level,
                       const bool clearHistory,
                       const int historyDepth,
                       const bool pack,
                       const int packDepth)
{
    if (ThreadBase::logThread())
        return true;

    auto thread = new ThreadLog(pathToBin,
                                consumer,
                                console,
                                level,
                                clearHistory,
                                historyDepth,
                                pack,
                                packDepth);
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

    logThread->postMessage1<MessageLog>(messageText,
                                        messageTemplate.number,
                                        messageTemplate.level,
                                        messageTemplate.subSystem,
                                        messageTemplate.messageType);
}

}
