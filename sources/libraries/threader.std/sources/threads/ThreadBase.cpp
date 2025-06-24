#include "ThreadBase.h"

#include "ListThreads.h"
#include "MessageLog.h"
#include "ThreadTimer.h"

#include "DateTimeUtils.h"

// #include <QTextCodec>

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
int ThreadBase::_logLevel          = 9;

ThreadBase::ThreadBase(MessageConsumer *parent,
                       const std::string &threadName,
                       const ThreadRunMode &threadRunMode)
    // : QThread(nullptr)
    : StdThreadWrapper()
    , MessageConsumer()
    , _parentThread(parent)
    , _threadName(threadName)
    , _thisThreadId(0)
    , _aliveCheckedUtc(DateTimeUtils::msecsSinceEpochLocal())
    , _waitCount(0)
    , _nextDestroyTerminatedThreadsTickCount(
          DateTimeUtils::getNextTickCount(TIMEOUT_COLLECT_TERMINATED_THREADS_MILLISECONDS))
    , _nextAccumulateStatisticTickCount(DateTimeUtils::getTickCount())
    , _nextCallIdle(0)

    , _threadRunMode(threadRunMode)
    // , _eventLoop(nullptr)
    // , _timerEventLoop(nullptr)
    , _mutex(new std::mutex())
{
    // _pollerThread.moveToThread(this);
    // if (ThreadRunMode::EventLoop == _threadRunMode)
    //     this->moveToThread(this);

    _polling.setThreadName(threadName);
}

ThreadBase::~ThreadBase()
{
    if (_mutex)
        delete _mutex;
}

MessageConsumer *ThreadBase::parentThread() const
{
    return _parentThread;
}

long ThreadBase::thisThreadId() const
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

ThreadBase::ThreadRunMode ThreadBase::threadRunMode() const { return _threadRunMode; }

void ThreadBase::setThreadRunMode(const ThreadRunMode &threadRunMode)
{
    if (isRunning())
        return;

    _threadRunMode = threadRunMode;

    if (ThreadRunMode::EventLoop == _threadRunMode)
    {
        // this->moveToThread(this);
    }
}

// std::string ThreadBase::threadClassName()
// {

//     return typeid(*this);
// }

long ThreadBase::threadId()
{
#ifdef linux
    return syscall(SYS_gettid);
#endif
#ifdef _WIN32
    return GetCurrentThreadId();
#endif
}

uint64_t ThreadBase::startedTickCount() const
{
    return _startedTickCount;
}

uint64_t ThreadBase::startedUtc() const
{
    return _startedUtc;
}

Polling *ThreadBase::polling() { return &_polling; }

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
// #if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
//     qsrand(uint(clock()));
// #endif

    if (ThreadsStaticticCollector::instance())
        ThreadsStaticticCollector::instance()->registerThread(this);

    _thisThreadId = -1;

    switch (_threadRunMode) {
        case ThreadRunMode::Polling:

            // connect(&_pollerThread, &PollerThread::signalTerminateThread, this,
            //         &ThreadBase::slotTerminateThread, Qt::DirectConnection);
            // connect(&_pollerThread, &PollerThread::signalTerminateChildThreads, this,
            //         &ThreadBase::slotTerminateChildThreads, Qt::DirectConnection);
            // connect(&_pollerThread, &PollerThread::signalWakeUpThread, this,
            //         &ThreadBase::slotWakeUpThread, Qt::DirectConnection);

            _pollerThread.setSignalHandler(this);
            _polling.registerPoller(&_pollerThread);

            break;
        case ThreadRunMode::EventLoop:
            // _eventLoop = new QEventLoop();
            // _eventLoop->moveToThread(this);

            // _timerEventLoop = new QTimer();
            // _timerEventLoop->moveToThread(this);
            // _timerEventLoop->setInterval(static_cast<int>(timeout()));
            // connect(_timerEventLoop, &QTimer::timeout, this, &ThreadBase::slotTimerEventLoop,
            //         Qt::AutoConnection);
            // _timerEventLoop->start();

            // connect(this, &ThreadBase::signalTerminate, this, &ThreadBase::slotTerminateThread,
            //         Qt::AutoConnection);

            // connect(this, &ThreadBase::signalTerminateChildThreads, this,
            //         &ThreadBase::slotTerminateChildThreads, Qt::AutoConnection);

            // connect(this, &ThreadBase::signalWakeUp, this, &ThreadBase::slotWakeUpThread,
            //         Qt::AutoConnection);
            break;
    }
}

void ThreadBase::finalizeThread()
{
    if (ThreadsStaticticCollector::instance())
        ThreadsStaticticCollector::instance()->unregisterThread(this);

    // if (_eventLoop)
    //     delete _eventLoop;

    // if (_timerEventLoop)
    //     delete _timerEventLoop;
}

void ThreadBase::onThreadStarted() {}

void ThreadBase::onThreadFinishing() {}

void ThreadBase::onThreadFinished() {}

void ThreadBase::onIdle() {}

void ThreadBase::onBeforeWaitEvents() {}

void ThreadBase::onAfterWaitEvents()
{
    destroyTerminatedChildThreads(false);

    if (ThreadRunMode::EventLoop == _threadRunMode)
        onIdle();

    accumulateStatistic();
}

void ThreadBase::run()
{
    initializeThread();

    _thisThreadId = threadId();

    _startedTickCount = DateTimeUtils::getTickCount();
    _startedUtc       = DateTimeUtils::msecsSinceEpochLocal();

    onThreadStarted();

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Daemon"
    MESSAGE_TEMPLATE(50, 0, Debug, "Идентификатор потока [%s]: %d");
#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Common"

    writeLog(Message50, STRLOG(threadName()), threadId());

    startChildThreads();

    if (ThreadRunMode::Polling == _threadRunMode) {
        while (!isTerminated()) {
            onBeforeWaitEvents();

            waitEvents();

            onAfterWaitEvents();
        }
    }
    else {
        // _eventLoop->exec();
    }

    onThreadFinishing();

    terminateChildThreads();

    processMessages();

    onThreadFinished();

    finalizeThread();

    auto *ownerThread = parentThread();
    if (ownerThread)
        ownerThread->postEventChildThreadTerminated();
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
                              isTerminated());
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
    if (!thread->isRunning() && startNow)
        thread->start();

    return thread;
}

bool ThreadBase::terminateChildThread(ThreadBase *thread)
{
    if (!thread)
        return false;

    if (thread->isRunning())
    {
        thread->postTerminateEvent();
        while (!thread->isFinished()) {
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
            if ((*iterator)->isFinished())
                iterator = _childThreadsList.erase(iterator);
            else
                iterator++;
        }

        _nextDestroyTerminatedThreadsTickCount =
            DateTimeUtils::getNextTickCount(TIMEOUT_COLLECT_TERMINATED_THREADS_MILLISECONDS);
    }
}

void ThreadBase::waitEvents() { waitEvents(timeout()); }

void ThreadBase::waitEvents(uint timeout)
{
    if (isTerminated())
        return;

    // ожидание голосования
    // результаты положительного голосования обрабатыватся внутри функции
    // и вызываются соответствующие слоты
    // остальные результаты отдаются на обработку снаружи
    auto pollResult = _polling.poll(timeout);

    // если произошла ошибка
    if (pollResult < 0) {
        processError("Ошибка ожидания событий", errno);
    }

    // если выход по таймауту или нужно вызывать Idle
    if (0 == pollResult || _nextCallIdle < DateTimeUtils::getTickCount()) {
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

    QTextCodec *codec = QTextCodec::codecForName("CP1251");
    QString result    = codec->toUnicode(messageBuffer).trimmed();

    LocalFree(messageBuffer);

    return result;
#endif

#ifdef linux
    return std::string(strerror(errorNo));
#endif

    return "";
}

void ThreadBase::processError(const std::string &errorTitle, int errorNo)
{
#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Error"

    MESSAGE_TEMPLATE(99999, 0, Error, "%s: %d - %s");

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Common"

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

void ThreadBase::writeLog(MessageLog::Ptr message, MessageConsumer *receiver)
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
    char resultPtr[1024];
    memset(resultPtr, 0, sizeof(resultPtr));
    auto resultCount = vsnprintf(resultPtr, sizeof(resultPtr), STRLOG(messageTemplate.text), args);
    va_end(args);

    std::string messageText;
    if (resultCount > 0) {
        messageText = std::string(resultPtr);
    }
    else {
        messageText = "Не удалось выполнить форматирование строки";
    }
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
    char resultPtr[1024];
    memset(resultPtr, 0, sizeof(resultPtr));
    auto resultCount = vsnprintf(resultPtr, sizeof(resultPtr), templateString, args);
    va_end(args);

    std::string messageText;
    if (resultCount > 0) {
        messageText = std::string(resultPtr);
    }
    else {
        messageText = "Не удалось выполнить форматирование строки";
    }
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
        slotTerminateThread();
        break;
    case (Signal)ThreadSignals::SignalWakeUp:
        slotWakeUpThread();
        break;
    case (Signal)ThreadSignals::SignalTerminateChildThreads:
        slotTerminateChildThreads();
        break;
    default:
        break;
    }
}

// QEventLoop *ThreadBase::eventLoop() const { return _eventLoop; }

ThreadBase * ThreadBase::startThreadTimer(const std::string &timerName,
                                         const int &delayMsecs,
                                         const bool multiShot)
{
    auto thread = new ThreadTimer(this, timerName, delayMsecs, multiShot);
    registerAndStartChildThread(thread);
    return thread;
}

bool ThreadBase::stopTimer(const std::string &timerName)
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
    if (isFinished())
        return;

    switch (_threadRunMode) {
        case ThreadRunMode::Polling:
            _pollerThread.sendSignalWakeUp();
            break;
        case ThreadRunMode::EventLoop:
            // emit signalWakeUp();
            break;
    }
}

void ThreadBase::signalEventTerminate()
{
    if (isFinished())
        return;

    switch (_threadRunMode) {
        case ThreadRunMode::Polling:
        _pollerThread.sendSignalTerminate();
            break;
        case ThreadRunMode::EventLoop:
            // emit signalTerminate();
            break;
    }
}

void ThreadBase::signalEventChildThreadTerminated()
{
    if (isFinished())
        return;

    switch (_threadRunMode) {
        case ThreadRunMode::Polling:
        _pollerThread.sendSignalTerminateChildThreads();
            break;
        case ThreadRunMode::EventLoop:
            // emit signalTerminateChildThreads();
            break;
    }
}

uint ThreadBase::timeout() const
{
    std::lock_guard<std::mutex> lock(*_mutex);
    return _timeout;
}

void ThreadBase::setTimeout(uint timeout)
{
    std::lock_guard<std::mutex> lock(*_mutex);
    _timeout = timeout;
}

void ThreadBase::slotTerminateThread()
{
    // if (ThreadRunMode::EventLoop == _threadRunMode && _eventLoop)
    // {
    //     _eventLoop->exit();
    // }

    terminate();
}

void ThreadBase::slotTerminateChildThreads()
{
    destroyTerminatedChildThreads(true);
}

void ThreadBase::slotWakeUpThread()
{
    if (threadRunMode() == ThreadRunMode::EventLoop)
    {
        DateTimeUtils::sleepMilliseconds(0);
    }
    processMessages();
}

void ThreadBase::slotTimerEventLoop()
{
    onAfterWaitEvents();
}

}
