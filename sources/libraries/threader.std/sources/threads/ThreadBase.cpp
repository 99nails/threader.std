#include "ThreadBase.h"

#include "ListThreads.h"
#include "LogMessagesTemplates.h"
#include "MessageLog.h"
#include "ThreadTimer.h"

#include "DateTimeUtils.h"
#include "ThreadUtils.h"
#include "SignalHandler.h"
#include "StdStringUtils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef linux
    #include <fcntl.h>
    #include <sys/syscall.h>
    #include <unistd.h>
#endif

#include <iostream>

namespace Threader::Threads {

using namespace Threader::Utils;

#define STACK_LEVEL 32

const int ThreadBase::WAIT_RESULT_TIMEOUT         = 0;
const int ThreadBase::WAIT_RESULT_ERROR           = -1;
const int ThreadBase::WAIT_RESULT_NOT_INITIALIZED = -2;

const int ThreadBase::TIMEOUT_COLLECT_TERMINATED_THREADS_MILLISECONDS = 1000;
const int ThreadBase::TIMEOUT_ACCUMULATE_STATISTIC_MILLISECONDS       = 10000;

const int ThreadBase::TIMEOUT_FINISH_THREADS = 1000;

ThreadBase *ThreadBase::_logThread = nullptr;
int ThreadBase::_logLevel = 9;

ThreadBase::ThreadBase(MessageConsumer *parent,
                       const std::string &threadName)
    : StdThreadWrapper()
    , MessageConsumer()
    , _parentThread(parent)
    , _threadName(threadName)
    , _thisThreadId(0)
    , _aliveCheckedUtc(DateTimeUtils::msecsSinceEpochLocal())
    , _waitCount(0)
    , _nextDestroyTerminatedThreadsTickCount(
          DateTimeUtils::getNextTickCount(TIMEOUT_COLLECT_TERMINATED_THREADS_MILLISECONDS))
    , _nextAccumulateStatisticTickCount(0)
    , _nextCallIdle(0)

    , _mutexTimeout(new std::mutex())
{
    _polling.setThreadName(threadName);
}

ThreadBase::~ThreadBase()
{
    if (_mutexTimeout)
        delete _mutexTimeout;
}

MessageConsumer *ThreadBase::parentThread() const
{
    return _parentThread;
}

THREAD_ID ThreadBase::thisThreadId() const
{
    return _thisThreadId;
}

std::string ThreadBase::threadName() const
{
    return _threadName;
}

void ThreadBase::setThreadName(const std::string &threadName)
{
    _threadName = threadName;
    _polling.setThreadName(threadName);
}

std::string ThreadBase::threadClassName()
 {

     return demangle(typeid(*this).name());
 }

THREAD_ID ThreadBase::threadId()
{
    return ThreadUtils::currentThreadId();
}

uint64_t ThreadBase::startedTickCount() const
{
    return _startedTickCount;
}

uint64_t ThreadBase::startedUtc() const
{
    return _startedUtc;
}

Polling *ThreadBase::polling()
{
    return &_polling;
}

void ThreadBase::postMessage(const MessageBase::Ptr &message)
{
    MessageConsumer::postMessage(message);
    signalEventWakeUp();
}

void ThreadBase::postMessages(const MessageList &messagesList)
{
    MessageConsumer::postMessages(messagesList);
    signalEventWakeUp();
}

void ThreadBase::postTerminateEvent()
{
    signalEventTerminate();
}

void ThreadBase::postEventChildThreadTerminated()
{
    signalEventChildThreadTerminated();
}

void ThreadBase::initializeThread()
{
    if (ThreadsStaticticCollector::instance())
        ThreadsStaticticCollector::instance()->registerThread(this);

    _thisThreadId = 0;

    _pollerThread.setSignalHandler(this);
    _polling.registerPoller(&_pollerThread);

    accumulateStatistic();
}

void ThreadBase::finalizeThread()
{
    if (ThreadsStaticticCollector::instance())
        ThreadsStaticticCollector::instance()->unregisterThread(this);
}

void ThreadBase::onThreadStarted() {}

void ThreadBase::onThreadFinishing() {}

void ThreadBase::onThreadFinished() {}

void ThreadBase::onIdle() {}

void ThreadBase::onBeforeWaitEvents() {}

void ThreadBase::onAfterWaitEvents()
{
    destroyTerminatedChildThreads(false);

    accumulateStatistic();
}

void ThreadBase::runEventLoop()
{
    while (!isThreadTerminated())
    {
        onBeforeWaitEvents();

        waitEvents();

        onAfterWaitEvents();
    }
}

void ThreadBase::execute()
{
    initializeThread();

    _startedTickCount = DateTimeUtils::getTickCount();
    _startedUtc = DateTimeUtils::msecsSinceEpochLocal();

    _thisThreadId = threadId();

    onThreadStarted();

    writeLog(Message13, STRLOG(threadName()), threadId());

    startChildThreads();

    runEventLoop();

    onThreadFinishing();

    terminateChildThreads();

    processMessages();

    onThreadFinished();

    finalizeThread();

    auto *ownerThread = parentThread();
    if (ownerThread)
        ownerThread->postEventChildThreadTerminated();

    writeLog(Message14, STRLOG(threadName()), threadId());
}

bool ThreadBase::accumulateStatistic()
{
    auto nowTickCount = DateTimeUtils::getTickCount();

    if (nowTickCount <= _nextAccumulateStatisticTickCount)
        return false;

    _nextAccumulateStatisticTickCount = nowTickCount + TIMEOUT_ACCUMULATE_STATISTIC_MILLISECONDS;

    ThreadsStaticticCollector *list = ThreadsStaticticCollector::instance();
    if (!list)
        return false;

    list->accumulateStatistic(this,
                              _thisThreadId,
                              _threadName,
                              _startedUtc,
                              DateTimeUtils::msecsSinceEpochLocal(),
                              nowTickCount,
                              polling()->waitCount(),
                              queue()->count(),
                              isThreadTerminated()
                              );
    return true;
}

int ThreadBase::childthreadsCount()
{
    return _childThreadsList.size();
}

ThreadBase *ThreadBase::childThread(int index)
{
    return _childThreadsList.at(index);
}

ThreadBase *ThreadBase::registerAndStartChildThread(ThreadBase *thread,
                                                    bool startNow)
{
    if (!thread)
        return nullptr;

    _childThreadsList.push_back(thread);
    if (!thread->isThreadRunning() && startNow)
        thread->startThread();

    return thread;
}

bool ThreadBase::terminateChildThread(ThreadBase *thread)
{
    if (!thread)
        return false;

    if (thread->isThreadRunning())
    {
        thread->postTerminateEvent();
        while (!thread->isThreadFinished()) {
            processMessages();
            DateTimeUtils::sleepMilliseconds(0);
        }

        accumulateStatistic();
    }
    processMessages();
    return true;
}

void ThreadBase::startChildThreads() {}

void ThreadBase::terminateChildThreads()
{
    for (int i = _childThreadsList.size() - 1; i >= 0; i--)
    {
        auto thread = _childThreadsList[i];
        terminateChildThread(thread);
    }

    for (int i = _childThreadsList.size() - 1; i >= 0; i--)
    {
        auto thread = _childThreadsList[i];
        delete thread;
    }

    _childThreadsList.clear();
}

void ThreadBase::onDestroyTerminatedChildThread(ThreadBase *thread)
{
    (void)thread;
}

void ThreadBase::destroyTerminatedChildThreads(bool rightNow)
{
    if (rightNow
        || _nextDestroyTerminatedThreadsTickCount <= DateTimeUtils::getTickCount())
    {
        auto iterator = _childThreadsList.begin();
        while (iterator != _childThreadsList.end())
        {
            if ((*iterator)->isThreadFinished())
            {
                onDestroyTerminatedChildThread(*iterator);
                iterator = _childThreadsList.erase(iterator);
            }
            else
                iterator++;
        }

        _nextDestroyTerminatedThreadsTickCount =
            DateTimeUtils::getNextTickCount(TIMEOUT_COLLECT_TERMINATED_THREADS_MILLISECONDS);
    }
}

void ThreadBase::waitEvents()
{
    waitEvents(timeout());
}

void ThreadBase::waitEvents(uint32_t timeout)
{
    if (isThreadTerminated())
        return;

    // ожидание голосования
    // результаты положительного голосования обрабатыватся внутри функции
    // и вызываются соответствующие методы обработчиков головосования
    // остальные результаты отдаются на обработку снаружи
    auto pollResult = _polling.poll(timeout);

    // если произошла ошибка
    if (pollResult < 0) {
        processError("Ошибка ожидания событий", errno);
    }

    // если выход по таймауту или нужно вызывать Idle
    if (0 == pollResult || _nextCallIdle < DateTimeUtils::getTickCount()) {
        if (!isThreadTerminated())
            onIdle();
        _nextCallIdle = DateTimeUtils::getNextTickCount(timeout);
    }
    _aliveCheckedUtc = DateTimeUtils::msecsSinceEpochLocal();
}

std::string ThreadBase::errorString(int errorNo)
{
    if (0 == errorNo)
        errorNo = ThreadBase::errorNo();
#ifdef _WIN32
    LPSTR messageBuffer = nullptr;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorNo, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    return std::string(messageBuffer);
#endif

#ifdef linux
    return std::string(strerror(errorNo));
#endif

    return "";
}

void ThreadBase::processError(const std::string &errorTitle, int errorNo)
{
    if (errorNo < 0)
        errorNo = errno;

    writeLog(Message99999, STRLOG(errorTitle), errorNo, strerror(errorNo));
}

void ThreadBase::setLogThread(ThreadBase *thread, int level)
{
    _logThread = thread;
    _logLevel  = level;
}

int ThreadBase::logLevel()
{
    return _logLevel;
}

void ThreadBase::setLogLevel(int level)
{
    _logLevel = level;
}

ThreadBase *ThreadBase::logThread()
{
    return _logThread;
}

void ThreadBase::writeLog(MessageLog::Ptr message,
                          MessageConsumer *receiver)
{
    if (!message)
        return;
    if (message->level() > _logLevel)
        return;

    if (_logThread)
        receiver = _logThread;
    if (!receiver)
        receiver = _parentThread;
    if (!receiver)
        return;
    receiver->postMessage(message);
}

void ThreadBase::writeLog(MessageConsumer *receiver,
                          const MessageLogTemplate messageTemplate,
                          ...)
{
    if (messageTemplate.level > _logLevel)
        return;

    va_list args;
    va_start(args, messageTemplate);
    char resultPtr[1024];
    memset(resultPtr, 0, sizeof(resultPtr));
    auto resultCount = vsnprintf(resultPtr, sizeof(resultPtr), STRLOG(messageTemplate.text), args);
    va_end(args);

    std::string message;
    if (resultCount > 0) {
        message = std::string(resultPtr);
    }
    else {
        message = "Не удалось выполнить форматирование строки";
    }
    auto messageLog = std::make_shared<MessageLog>(message,
                                                   messageTemplate.number,
                                                   messageTemplate.level,
                                                   messageTemplate.subSystem,
                                                   messageTemplate.messageType);
    writeLog(messageLog, receiver);
}

void ThreadBase::writeLog(const MessageLogTemplate messageTemplate, ...)
{
    if (messageTemplate.level > _logLevel || !_logThread)
        return;

    va_list args;
    va_start(args, messageTemplate);
    auto messageText = format(messageTemplate.text.c_str(), args);
    va_end(args);

    auto message = std::make_shared<MessageLog>(messageText,
                                                messageTemplate.number,
                                                messageTemplate.level,
                                                messageTemplate.subSystem,
                                                messageTemplate.messageType);
    _logThread->postMessage(message);
}

void ThreadBase::outputDebug(const char *templateString, ...)
{
    va_list args;
    va_start(args, templateString);
    auto messageText = format(templateString, args);
    va_end(args);
    std::cout << "\r\n" << messageText << "\r\n";
}

int ThreadBase::errorNo()
{
#ifdef linux
    return errno;
#endif
#ifdef _WIN32
    return GetLastError();
#endif

}

void ThreadBase::signalProcess(Signal signal)
{
    switch (signal) {
    case (Signal)ThreadSignals::SignalTerminate:
        terminateThread();
        break;
    case (Signal)ThreadSignals::SignalWakeUp:
        processMessages();
        break;
    case (Signal)ThreadSignals::SignalTerminateChildThreads:
        destroyTerminatedChildThreads(true);
        break;
    default:
        break;
    }
}

void ThreadBase::processMessage(const MessageBase::Ptr &)
{
}

ThreadBase * ThreadBase::startThreadTimer(const std::string &timerName,
                                         const int &delayMsecs,
                                         const bool multiShot)
{
    auto thread = new ThreadTimer(this,
                                  timerName,
                                  delayMsecs,
                                  multiShot);

    registerAndStartChildThread(thread);
    return thread;
}

bool ThreadBase::stopThreadTimer(const std::string &timerName)
{
    bool result = false;

    for (size_t i = 0; i < _childThreadsList.size(); i++) {
        auto thread = dynamic_cast<ThreadTimer *>(_childThreadsList.at(i));
        if (!thread)
            continue;

        if (thread->timerName() == timerName) {
            thread->postTerminateEvent();
            result = true;
        }
    }
    return result;
}

int ThreadBase::stopAllTimers()
{
    int result = 0;

    for (size_t i = 0; i < _childThreadsList.size(); i++) {
        auto thread = dynamic_cast<ThreadTimer *>(_childThreadsList.at(i));
        if (!thread)
            continue;

        thread->postTerminateEvent();
        result++;
    }
    return result;
}

void ThreadBase::signalEventWakeUp()
{
    if (this->isThreadFinished())
        return;

    _pollerThread.sendSignalWakeUp();
}

void ThreadBase::signalEventTerminate()
{
    if (isThreadFinished())
        return;

    _pollerThread.sendSignalTerminate();
}

void ThreadBase::signalEventChildThreadTerminated()
{
    if (isThreadFinished())
        return;

    _pollerThread.sendSignalTerminateChildThreads();
}

uint32_t ThreadBase::timeout() const
{
    std::lock_guard<std::mutex> lock(*_mutexTimeout);
    return _timeout;
}

void ThreadBase::setTimeout(uint32_t timeout)
{
    std::lock_guard<std::mutex> lock(*_mutexTimeout);
    _timeout = timeout;
}

}
