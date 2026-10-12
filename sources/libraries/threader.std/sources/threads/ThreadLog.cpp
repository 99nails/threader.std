#include "ThreadLog.h"

#include "DateTimeUtils.h"
#include "LogMessagesTemplates.h"
#include "MessageLog.h"
#include "StdStringUtils.h"
#include "ThreadLogCleaner.h"

namespace Threader::Threads {

using namespace Threader::Utils;

constexpr char MESSAGE_NAME_LOG_LEVEL[] = "Message.Log.Level";

ThreadLog *ThreadLog::_instance = nullptr;

ThreadLog::ThreadLog(const std::string &pathToBin,
                     MessageConsumer *consumer,
                     const bool console,
                     const int level,
                     const bool clearHistory,
                     const int historyDepth,
                     const bool pack,
                     const int packDepth)
    : ThreadBase(nullptr, "Thread.Log")
    , _logger(nullptr)
    , _consumer(consumer)
    , _threadLogCleaner(nullptr)
{
    _instance = this;
    _logger = new Logger(console, pathToBin, level, logDetailsAllWithoutDate);
    ThreadBase::setLogThread(this, _logger->level());

    if (clearHistory)
    {
        _threadLogCleaner =  new ThreadLogCleaner(this,
                                                  _logger->pathToLog(),
                                                  historyDepth,
                                                  pack,
                                                  packDepth);
    }

    setTimeout(2000);
}

ThreadLog::~ThreadLog()
{
    if (ThreadBase::logThread())
        ThreadBase::setLogThread(nullptr);

    if (_logger)
    {
        delete _logger;
        _logger = nullptr;
    }
}

void ThreadLog::setLevel(const int level)
{
    if (!_instance)
        return;

    _instance->postMessage1<MessageInt>(MESSAGE_NAME_LOG_LEVEL, level);
}

void ThreadLog::flush()
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

    if (_threadLogCleaner)
        registerAndStartChildThread(_threadLogCleaner);
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

    flush();
}

void ThreadLog::onIdle()
{
    flush();
}

void ThreadLog::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    const auto messageName = message->name();

    if (MessageLog::MESSAGE_NAME == messageName)
    {
        doMessageLog(std::dynamic_pointer_cast<MessageLog>(message));
    }
    else if (MessageWriteToFile::MESSAGE_NAME == messageName)
    {
        doMessageWriteToFile(std::dynamic_pointer_cast<MessageWriteToFile>(message));
    }
    if (MESSAGE_NAME_LOG_LEVEL == messageName)
    {
        doMessageLogLevel(std::dynamic_pointer_cast<MessageInt>(message));
    }
}

void ThreadLog::doMessageLog(const MessageLog::Ptr &message)
{
    if (!message || !_logger)
        return;

    _logger->collectMessage(message->created(),
                   message->number(),
                   message->level(),
                   message->messageType(),
                   message->subSystem(),
                   message->threadId(),
                   message->value());

    if (_consumer && _logger->level() >= message->level())
        _consumer->postMessage(message);
}

void ThreadLog::doMessageWriteToFile(const MessageWriteToFile::Ptr &message)
{
    if (!message || !_logger)
        return;

    switch (message->writeMode()) {
    case Threader::Threads::MessageWriteToFile::WriteMode::Truncate:
        _logger->snapshot(message->fileName(), message->value());
        break;
    case Threader::Threads::MessageWriteToFile::WriteMode::Append:
        _logger->append(message->fileName(), message->value());
        break;
    }
}

void ThreadLog::doMessageLogLevel(const MessageInt::Ptr &message)
{
    if (!message)
        return;

    if (!_logger)
        return;

    _logger->setLevel(message->value());

    _logger->collectMessage(DateTimeUtils::msecsSinceEpochLocal(),
                            Message3.number,
                            Message3.level,
                            Message3.messageType,
                            Message3.subSystem,
                            thisThreadId(),
                            format(Message3.text.c_str(),
                                   _logger->level()));
}

}
