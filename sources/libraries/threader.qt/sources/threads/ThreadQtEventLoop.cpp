#include "ThreadQtEventLoop.h"

#include <QtGlobal>

namespace Threader::Threads::QtEventLoop
{

ThreadQtEventLoop::ThreadQtEventLoop(MessageConsumer *parent,
                                     const std::string &threadName)
    : QThread()
    , ThreadBase(parent, threadName)
    , _timerIdle(nullptr)
{
}

void ThreadQtEventLoop::startThread()
{
    QThread::start();
}

void ThreadQtEventLoop::initializeThread()
{
    ThreadBase::initializeThread();

    _timerIdle = new QTimer();
    _timerIdle->moveToThread(this);
    _timerIdle->setInterval(static_cast<int>(timeout()));
    connect(_timerIdle, &QTimer::timeout,
            this, &ThreadQtEventLoop::slotTimerEventLoop,
            Qt::AutoConnection);
    _timerIdle->start();

    connect(this, &ThreadQtEventLoop::signalTerminate,
            this, &ThreadQtEventLoop::slotTerminateThread,
            Qt::AutoConnection);

    connect(this, &ThreadQtEventLoop::signalTerminateChildThreads,
            this, &ThreadQtEventLoop::slotTerminateChildThreads,
            Qt::AutoConnection);

    connect(this, &ThreadQtEventLoop::signalWakeUp,
            this, &ThreadQtEventLoop::slotWakeUpThread,
            Qt::AutoConnection);
}

void ThreadQtEventLoop::finalizeThread()
{
    if (_timerIdle)
    {
        if (_timerIdle->isActive())
            _timerIdle->stop();
        delete _timerIdle;
        _timerIdle = nullptr;
    }

    ThreadBase::finalizeThread();
}

void ThreadQtEventLoop::runEventLoop()
{
    QThread::exec();
}

void ThreadQtEventLoop::run()
{
    setIsRunning(true);

    execute();

    setIsRunning(false);
}

void ThreadQtEventLoop::signalEventWakeUp()
{
    if (this->isThreadFinished())
        return;

    emit signalWakeUp();
}

void ThreadQtEventLoop::signalEventTerminate()
{
    if (this->isThreadFinished())
        return;

    emit signalTerminate();
}

void ThreadQtEventLoop::signalEventChildThreadTerminated()
{
    if (this->isThreadFinished())
        return;

    emit signalTerminateChildThreads();
}

void ThreadQtEventLoop::slotTerminateThread()
{
    terminateThread();
    QThread::exit();
}

void ThreadQtEventLoop::slotTerminateChildThreads()
{
    destroyTerminatedChildThreads();
}

void ThreadQtEventLoop::slotWakeUpThread()
{
    processMessages();
}

void ThreadQtEventLoop::slotTimerEventLoop()
{
    onIdle();

    onAfterWaitEvents();
}

}


