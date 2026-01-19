#include "ThreadMainQtEventLoop.h"

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Main"

MESSAGE_TEMPLATE(7000, 0, MessageType::Message, "ThreadId %d: Событие таймера")

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Common"

ThreadMainQtEventLoop::ThreadMainQtEventLoop(const std::string &applicationName,
                                             const std::string &description)
    : ThreadMainQt(nullptr, applicationName, description, "Thread.Main")
    , _timer(nullptr)
{
}

ThreadMainQtEventLoop::~ThreadMainQtEventLoop()
{
}

void ThreadMainQtEventLoop::onThreadStarted()
{
    ThreadMainQt::onThreadStarted();

    _timer = new QTimer(this);
    _timer->moveToThread(this);
    _timer->setSingleShot(false);
    _timer->start(1000);
    connect(_timer, &QTimer::timeout, this, &ThreadMainQtEventLoop::slotOnTimer);
}

void ThreadMainQtEventLoop::slotOnTimer()
{
    writeLog(Message7000, threadId());
}
