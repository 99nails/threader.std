#pragma once

#include "ThreadMainQt.h"

#include <QObject>

using namespace Threader::Threads::QtEventLoop;

class ThreadMainQtEventLoop : public ThreadMainQt
{
    Q_OBJECT
public:
    explicit ThreadMainQtEventLoop(const std::string &applicationName,
                                   const std::string &description);
    virtual ~ThreadMainQtEventLoop();

protected:
    void onThreadStarted() override;

private:
    QTimer *_timer;

private slots:
    void slotOnTimer();
};

