#pragma once

#include "MessageBase.h"
#include "MessageConsumer.h"
#include "MessageLog.h"
#include "PollerThread.h"

#include "StdThreadWrapper.h"

#ifdef linux
#include "PollingPosix.h"
#endif

#ifdef _WIN
#include "PollingsWindows.h"
#endif

// #include "../threader_global.h"

#include <list>
#include <map>
#include <mutex>
#include <vector>

// #include <QDateTime>
// #include <QEventLoop>
// #include <QList>
// #include <QString>
// #include <QThread>
// #include <QTimer>
// #include <QVector>
// #include <QMutex>

#ifdef Q_OS_LINUX
#include <poll.h>
#endif

namespace Threader::Threads {

using namespace Threader::Utils;

class ThreadBase;

using ListThreadBase = std::list<ThreadBase *>;

using VectorThreadBase = std::vector<ThreadBase *>;

/**
 * @brief ThreadBase - Базовый класс потока системы
 * Реализует функционал:
 * - регистрации потока,
 * - отправки и обработки полученных классов сообщений,
 * - ожидания, отправки и обработки внешних событий от потоков и устройств на механизме poll-инга
 */
class ThreadBase : public StdThreadWrapper, public MessageConsumer, public ISignalHandler
{
    // Q_OBJECT
public:
    // enum class ThreadSignals : char
    // {
    //     EVENT_THREAD_TERMINATE        = 0,
    //     EVENT_THREAD_WAKEUP           = 1,
    //     EVENT_CHILD_THREAD_TERMINATED = 2
    // };

    /**
     * @brief режим ожидания событий потока
     */
    enum class ThreadRunMode {
        Polling, // на основе системных функций получения событий дексрипторов
        EventLoop // на основе QEventLoop
    };

    static const int TIMEOUT_FINISH_THREADS;

    /**
     * @brief ThreadBase - Конструктор потока с указанием владельца
     * @param parent - Владелец потока
     */
    explicit ThreadBase(MessageConsumer *parent = nullptr,
                        const std::string &threadName = std::string(),
                        const ThreadRunMode &threadRunMode = ThreadRunMode::Polling);

    /**
     * @brief ~ThreadBase - Деструктор потока
     */
    ~ThreadBase() override;

    /***********************************************************************************************
     * ПУБЛИЧНЫЕ СВОЙСТВА
    ************************************************************************************************/

    /**
     * @brief parentThread - Получение потока-владельца
     * @return
     */
    MessageConsumer *parentThread() const;

    /**
     * @brief thisThreadId - Получение собственного идентификатора потока
     * @return - Собственный идентификатор потока
     */
    long thisThreadId() const;

    /**
     * @brief threadClassName - Получение имени класса потока
     * @return - Имя класса потока
     */
    // std::string threadClassName();

    /**
     * @brief threadId - Получение идентификатора потока
     * @return - Идентификатор потока
     */
    static long threadId();

    /**
     * @brief startedTickCount - Получение счетчика системы на момент запуска потока
     * @return - Счетчик системы на момент запуска потока
     */
    uint64_t startedTickCount() const;

    /**
     * @brief started - Получение даты и времени запуска потока
     * @return - Дата и время запуска потока
     */
    uint64_t startedUtc() const;

    /**
     * @brief polling - Получение указателя на объект голосования
     * @return указатель на объект голосования
     */
    Polling *polling();

    /**
     * @brief logLevel - Получение уровня протоколирования
     * @return - Уровень протоколирования
     */
    static int logLevel();

    static void setLogLevel(int level);

    /***********************************************************************************************
    * ПОДСИСТЕМА ОТПРАВКИ СООБЩЕНИЙ ПОТОКУ
    ***********************************************************************************************/

    /**
     * @brief postMessage - Оправка сообщения потоку
     * @param message - Сообщения для потока
     */
    void postMessage(const MessageBase::Ptr &message) override;

    /**
     * @brief postMessages - Отправка потоку группы сообщений
     * @param messagesList - Группа сообщений
     */
    void postMessages(const MessageList &messagesList) override;

    /**
     * @brief postTerminateEvent - Отправка события завершения потока
     */
    void postTerminateEvent() override;

    /**
     * @brief postEventChildThreadTerminated - Отправка события завершения дочерних потоков
     */
    void postEventChildThreadTerminated() override;

    /**
     * @brief timeout - Получение времени ожидания событий голосования
     * @return - время ожидания событий голосования в миллисекундах
     */
    uint timeout() const;

    /**
     * @brief setTimeout - Установка времени ожидания событий голосования
     * @param timeout - время ожидания событий голосования в миллисекундах
     */
    void setTimeout(uint timeout);

    /**
     * @brief logThread - Получение зарегистрированного потока приема и обработки сообщений
     * протоколирования
     * @return - Поток приема и обработки сообщений протоколирования
     */
    static ThreadBase *logThread();

    /**
     * @brief writeLog - Отправка сообщения протоколирования потоку протоколирования
     * @param message - Сообщение протоколирования
     * @param receiver - Поток, обрабатывающий сообщения протоколирования. При значении NULL
     * отправка сообщения производится потоку-владельцу
     */
    virtual void writeLog(MessageLog::Ptr message, MessageConsumer *receiver = nullptr);

    /**
     * @brief writeLog - Отправка сообщения протоколирования потоку протоколирования
     * @param message - Шаблон сообщения протоколирования
     */
    static void writeLog(const MessageLogTemplate messageTemplate, ...);

    static void outputDebug(const char *templateString, ...);

    /**
     * @brief errorNo - получение номера последней ошибки
     * @return номер последней ошибки
     */
    static int errorNo();

    /**
     * @brief errorString - получение текстовой расшифровки номера ошибки
     * @param errorNo - номер ошибки
     * @return - текстовая расшифровка номера ошибки
     */
    static std::string errorString(int errorNo = 0);

    /**
     * @brief eventLoop - получение указателя на цикл обработки сообщений
     * @return
     */
    // QEventLoop *eventLoop() const;

    void signalProcess(Signal signal) override;

protected:
    /***********************************************************************************************
    * ПОДСИСТЕМА ВЫПОЛНЕНИЯ
    ***********************************************************************************************/

    /**
     * @brief initializeThread - Инициализация потока
     */
    virtual void initializeThread();

    /**
     * @brief FinalizeThread - Финализация потока
     */
    virtual void finalizeThread();

    /**
     * @brief onThreadStarted - Виртуальный метод вызываемый перед запуском потока.
     * Предназначен для перекрытия в дочерних потоках
     */
    void virtual onThreadStarted();

    /**
     * @brief onThreadFinishing - Виртуальный метод вызываемый перед остановкой потока.
     * Предназначен для перекрытия в дочерних потоках
     */
    void virtual onThreadFinishing();

    /**
     * @brief onThreadFinished - Виртуальный метод вызываемый перед остановкой потока.
     * Предназначен для перекрытия в дочерних потоках
     */
    void virtual onThreadFinished();

    /**
     * @brief onIdle - Виртуальный метод вызываемый по получении возможности таймаута при poll-инге.
     * Предназначен для перекрытия в дочерних потоках
     */
    void virtual onIdle();

    /**
     * @brief onBeforeWaitEvents - Заглушка вызова перед ожиданием событий
     * Используется для установки дополнительных событий потока в потомках
     */
    void virtual onBeforeWaitEvents();

    /**
     * @brief run - Главная функция потока
     */
    void run() override;

    /**
     * @brief accumulateStatistic - сбор статистики выполнения потоков
     */
    virtual bool accumulateStatistic();

    /**
     * @brief threadName - Получение имени потока
     * @return - Имя потока
     */
    std::string threadName() const;

    /**
     * @brief setThreadName - установка имени потока
     * @param threadName - новое имя потока
     */
    void setThreadName(const std::string &threadName);

    /**
     * @brief threadRunMode - получение режима ожидания событий потока
     * @return - режим ожидания событий потока
     */
    ThreadRunMode threadRunMode() const;

    /**
     * @brief setThreadRunMode - установка режима ожидания событий потока
     * @param threadRunMode - режим ожидания событий потока
     */
    void setThreadRunMode(const ThreadRunMode &threadRunMode);

    /***********************************************************************************************
    * ПОДСИСТЕМА УПРАВЛЕНИЯ ПОДЧИНЕННЫМИ ПОТОКАМИ
    ***********************************************************************************************/

    /**
     * @brief childthreadsCount - Получение количества подчиненных потоков
     * @return - Количество подчиненных потоков
     */
    int childthreadsCount();

    /**
     * @brief childThread - Получение подчиненного потока по индексу
     * @param index - Номер подчиненного потока
     * @return - Запрашиваемый поток
     */
    ThreadBase *childThread(int index);

    /**
     * @brief registerAndStartChildThread - Регистрация и запуск подчиненного потока
     * @param thread - Подчиненный поток
     * @return Зарегистрированный подчиненный поток
     */
    ThreadBase *registerAndStartChildThread(ThreadBase *thread, bool startNow = true);

    /**
     * @brief terminateChildThread - Остановка подчиненного потока
     * @param thread - Подчиненный поток
     * @return - Результат остановки
     */
    bool terminateChildThread(ThreadBase *thread);

    /**
     * @brief startChildThreads - Запуск подчиненных потоков
     * Вызывается при старте потока
     */
    virtual void startChildThreads();

    /**
     * @brief terminateChildThreads - Остановка всех зарегистрированных подчиненных потоков
     */
    virtual void terminateChildThreads();

    /**
     * @brief onDestroyTerminatedChildThread - метод, вызываемый перед уничтожением экземпляра
     * остановившегося потока
     * @param thread - Уничтожаемый поток
     */
    virtual void onDestroyTerminatedChildThread(ThreadBase *thread);

    /**
     * @brief destroyTerminatedThreads - Уничтожение остановившихся потокв
     */
    virtual void destroyTerminatedChildThreads(bool rightNow = false);

    /***********************************************************************************************
     * ПОДСИСТЕМА СОБЫТИЙ
    ************************************************************************************************/

    /**
     * @brief waitEvents - ожидание событий с текущим таймаутом
     */
    void waitEvents();

    /**
     * @brief waitEvents - ожидание событий
     * @param timeout - таймаут ожидания событий
     */
    virtual void waitEvents(uint timeout);

    /***********************************************************************************************
     * ПОДСИСТЕМА ПРОТОКОЛИРОВАНИЯ
    ************************************************************************************************/

    /**
     * @brief processError - Обработка ошибки потока
     * @param errorTitle - Заголовок ошибки в протоколе работы
     * @param errorNo - Номер ошибки
     */
    virtual void processError(const std::string &errorTitle, int errorNo = -1);

    /**
     * @brief setLogThread - Регистрация потока приема и обработки сообщений протоколирования
     * @param thread - Поток приема и обработки сообщений протоколирования
     * @param level - Уровень протоколирования
     */
    static void setLogThread(ThreadBase *thread, int level);

    /**
     * @brief writeLog - Отправка сообщения протоколирования потоку протоколирования
     * @param receiver - Поток-получатель сообщения протоколирования
     * @param messageTemplate - Шаблон сообщения
     */
    virtual void writeLog(MessageConsumer *receiver, const MessageLogTemplate messageTemplate, ...);

    /***********************************************************************************************
     * ПОДСИСТЕМА ТАЙМЕРОВ
    ************************************************************************************************/

    /**
     * @brief startThreadTimer - Запуск потока таймера
     * @param timerName - Имя сообщения таймера
     * @param delayMsecs - Задержка таймера в миллисекундах
     * @param multiShot - Признак множественного срабатывания таймера
     * @return - Указатель на поток таймера
     */
    virtual ThreadBase *startThreadTimer(const std::string &timerName,
                                         const int &delayMsecs,
                                         const bool multiShot);

    /**
     * @brief stopTimer - Остановка потока таймера
     * @param timerName - Имя сообщения таймера
     * @return - Признак остановки потока
     */
    virtual bool stopTimer(const std::string &timerName);

    /**
     * @brief stopAllTimers - Остановка всех потоков таймеров
     * @return - Количество остановленных потоков
     */
    virtual int stopAllTimers();

    virtual void onAfterWaitEvents();

private:
    static const int WAIT_RESULT_TIMEOUT;
    static const int WAIT_RESULT_ERROR;
    static const int WAIT_RESULT_NOT_INITIALIZED;
    static const int WAIT_EVENTS_TIMEOUT = 10000;

    static const int TIMEOUT_COLLECT_TERMINATED_THREADS_MILLISECONDS;
    static const int TIMEOUT_ACCUMULATE_STATISTIC_MILLISECONDS;

    /***********************************************************************************************
     * ВНУТРЕННЯЯ (PRIVATE) РЕАЛИЗАЦИЯ ПОДСИСТЕМЫ СОБЫТИЙ
    ************************************************************************************************/

    /**
     * @brief signalEventWakeUp - Отправка события пробуждения потока
     */
    void signalEventWakeUp();

    /**
     * @brief signalEventTerminate - Отправка события завершения потока
     */
    void signalEventTerminate();

    /**
     * @brief signalEventsChildThreadTerminated - Отправка события завершения потока this к потоку влядельцу parent
     */
    void signalEventChildThreadTerminated();

    /***********************************************************************************************
     * ПОДСИСТЕМА ПРОТОКОЛИРОВАНИЯ
    ************************************************************************************************/

    /**
     * @brief _logThread - Поток протоколирования
     */
    static ThreadBase *_logThread;

    static int _logLevel;

    /***********************************************************************************************
     * ЛОКАЛЬНЫЕ ПЕРЕМЕННЫЕ ПОТОКА
    ************************************************************************************************/

    /**
     * @brief _parentThread - поток-владелец
     */
    MessageConsumer *_parentThread;

    /**
     * @brief _threadName - Имя потока
     */
    std::string _threadName;

    /**
     * @brief _thisThreadId - Id экземпляра потока
     */
    Descriptor _thisThreadId;

    /**
     * @brief _startedTickCount - счетчик системы при запуске потока
     */
    uint64_t _startedTickCount;

    /**
     * @brief _started - Дата и время запуска потока
     */
    uint64_t _startedUtc;

    /**
     * @brief _aliveChecked - Дата и время установки признака жизни потока
     */
    uint64_t _aliveCheckedUtc;

    /**
     * @brief _polling - Класс голосования
     */
    Polling _polling;

    /**
     * @brief _pollerThread - Голосующий на входяший пайп сообщений
     */
    PollerThread _pollerThread;

    /**
     * @brief _childThreadsList - Список подчиненных потоков
     */
    VectorThreadBase _childThreadsList;

    /**
     * @brief _waitCount - Счетчик времени ожидания событий. Характеризует загруженность потока
     */
    uint64_t _waitCount;

    /**
     * @brief _nextDestroyTerminatedThreadsTickCount - Следующее время очистки остановленных потоков
     */
    uint64_t _nextDestroyTerminatedThreadsTickCount;

    /**
     * @brief _nextAccumulateStatisticTickCount - Следующее время сбора статистики выполнения потоков
     */
    uint64_t _nextAccumulateStatisticTickCount;

    /**
     * @brief _timeout - таймаут ожидания событий
     */
    uint _timeout = WAIT_EVENTS_TIMEOUT;

    /**
     * @brief _nextCallIdle - Время следующего вызова Idle
     */
    uint64_t _nextCallIdle;

    /**
     * @brief _subscribers - хранилище подписчиков на события
     */
    std::map<std::string, MessageConsumerVector> _subscribers;

    /**
     * @brief _threadRunMode - режим ожидания событий потока
     */
    ThreadRunMode _threadRunMode;

    /**
     * @brief _eventLoop - цикл обработки сообщений
     */
    // QEventLoop *_eventLoop;

    /**
     * @brief _timerEventLoop - таймер для вызова onIdle при схеме работы EventLoop
     */
    // QTimer *_timerEventLoop;
    std::mutex *_mutex = nullptr;

    // signals:
    //     void signalTerminate();
    //     void signalWakeUp();
    //     void signalTerminateChildThreads();

    // protected slots:
    void slotTerminateThread();
    void slotTerminateChildThreads();
    void slotWakeUpThread();
    void slotTimerEventLoop();
};

#define PRINT_THREAD_INFO \
    qInfo("Thread ID: %ld, Function: %s", threadId(), QString(Q_FUNC_INFO).toUtf8().constData());

} // namespace Threader::Threads
