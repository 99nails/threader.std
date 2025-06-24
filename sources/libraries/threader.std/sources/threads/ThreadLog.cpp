#include "ThreadLog.h"

// #include "DateTimeUtils.h"
#include "MessageLog.h"
#include "StdStringUtils.h"

namespace Threader::Threads {

using namespace Threader::Utils;

ThreadLog *ThreadLog::_instance = nullptr;

ThreadLog::ThreadLog(const std::string &pathToBin,
                     MessageConsumer *consumer)
    : ThreadBase(nullptr, "Thread.Log")
    , _logger(nullptr)
    , _consumer(consumer)
{
    _instance = this;
    _logger = new Logger(true, pathToBin, 9, logDetailsAll);

    ThreadBase::setLogThread(this, _logger->level());
}

ThreadLog::~ThreadLog()
{
    if (_logger)
    {
        delete _logger;
        _logger = nullptr;
    }
}

void Threader::Threads::ThreadLog::flush()
{
    if (_logger)
        _logger->flush();
}

void ThreadLog::onThreadStarted()
{
    _logger->collectMessage(DateTimeUtils::msecsSinceEpochLocal(),
                            Message0.number,
                            Message0.level,
                            Message0.messageType,
                            Message0.subSystem,
                            thisThreadId(),
                            format(Message0.text.c_str(),
                                   _logger->level(),
                                   _logger->pathToLog().c_str()));
}

void ThreadLog::onThreadFinishing()
{
    _logger->collectMessage(DateTimeUtils::msecsSinceEpochLocal(),
                            Message1.number,
                            Message1.level,
                            Message1.messageType,
                            Message1.subSystem,
                            thisThreadId(),
                            Message1.text);
}

void ThreadLog::onThreadFinished()
{
    _logger->collectMessage(DateTimeUtils::msecsSinceEpochLocal(),
                            Message2.number,
                            Message2.level,
                            Message2.messageType,
                            Message2.subSystem,
                            thisThreadId(),
                            Message2.text);
}

void ThreadLog::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    const auto messageName = message->name();

    if (MessageLog::MESSAGE_NAME == messageName)
    {
        const auto messageLog = std::dynamic_pointer_cast<MessageLog>(message);
        if (!messageLog || !_logger)
            return;

        _logger->collectMessage(messageLog->created(),
                       messageLog->number(),
                       messageLog->level(),
                       messageLog->messageType(),
                       messageLog->subSystem(),
                       messageLog->threadId(),
                       messageLog->value());

        if (_consumer && _logger->level() >= messageLog->level())
            _consumer->postMessage(messageLog);

    }
}

}
