#pragma once

#include "ThreadQtEventLoop.h"

#include <QObject>

using namespace Threader::Threads::QtEventLoop;

class ThreadMainQtEventLoop : public ThreadQtEventLoop
{
    Q_OBJECT
public:
    explicit ThreadMainQtEventLoop(const std::string &applicationName,
                                   const std::string &description);
    virtual ~ThreadMainQtEventLoop();

private:
    QTimer *_timer;

private slots:
    void slotOnTimer();
};

