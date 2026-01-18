#pragma once

#include "ThreadBase.h"

#include <QEventLoop>
#include <QObject>
#include <QThread>
#include <QTimer>

namespace Threader::Threads::QtEventLoop
{


using namespace Threader::Threads;


class ThreadQtEventLoop:
        public QThread,
        public ThreadBase
{
    Q_OBJECT
public:

    explicit ThreadQtEventLoop(MessageConsumer *parent,
                               const std::string &threadName = std::string());
    virtual ~ThreadQtEventLoop() = default;

    void startThread() override;

protected:
    void initializeThread() override;

    void finalizeThread() override;

    void runEventLoop() override;

private:
    /**
     * @brief _timerEventLoop - таймер для вызова onIdle при схеме работы EventLoop
     */
    QTimer *_timerIdle;


    // QThread interface
protected:
    void run() override;

    /***********************************************************************************************
     * ВНУТРЕННЯЯ РЕАЛИЗАЦИЯ ПОДСИСТЕМЫ СОБЫТИЙ
    ************************************************************************************************/

    /**
     * @brief signalEventWakeUp - Отправка события пробуждения потока
     */
    void signalEventWakeUp() override;

    /**
     * @brief signalEventTerminate - Отправка события завершения потока
     */
    void signalEventTerminate() override;

    /**
     * @brief signalEventsChildThreadTerminated - Отправка события завершения потока this к потоку влядельцу parent
     */
    void signalEventChildThreadTerminated() override;

signals:
    void signalTerminate();
    void signalWakeUp();
    void signalTerminateChildThreads();

protected slots:
    void slotTerminateThread();
    void slotTerminateChildThreads();
    void slotWakeUpThread();
    void slotTimerEventLoop();

};

}
