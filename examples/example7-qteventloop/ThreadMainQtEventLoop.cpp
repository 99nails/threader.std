#include "ThreadMainQtEventLoop.h"

ThreadMainQtEventLoop::ThreadMainQtEventLoop(const std::string &applicationName,
                                             const std::string &description)
    : ThreadQtEventLoop(nullptr, "Thread.Main")
    , _timer(new QTimer(this))
{
    connect(_timer, &QTimer::timeout, this, &ThreadMainQtEventLoop::slotOnTimer);
    _timer->setSingleShot(false);
    _timer->start(1000);

}

ThreadMainQtEventLoop::~ThreadMainQtEventLoop()
{

}

void ThreadMainQtEventLoop::slotOnTimer()
{

}
