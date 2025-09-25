#pragma once

#include "DateTimeUtils.h"
#include "ThreadsCommon.h"

#include <list>
#include <string>

namespace Threader::Utils {

/**
 * @brief MessageType - Тип сообщений (Сообщение, Предупреждение, Ошибка, Отладка)
 */
enum MessageType { Message, Attention, Warning, Error, Debug };

/**
 * @brief MessageToStringFlags - флаги отображения данных при форматировании сообщения
 * протоколирования в строку
 */
enum LogDetails {
    Date = 1,
    Time = 2,
    Type = 4,
    Number = 8,
    Subsystem = 16,
    ThreadId = 32,
    Text = 64
};

/**
 * @brief MessagesTypesNames
 */
const std::string MessagesTypesNames[MessageType::Debug + 1] = {"MSG", "ATT", "WRN", "ERR", "DBG"};

#pragma pack(push, 4)
struct MessageLogTemplate
{
    int number;
    int level;
    std::string subSystem;
    MessageType messageType;
    std::string text;
};
#pragma pack(pop)

#define LOG_SUBSYSTEM "Common"

/**
 * Макрос для задания шаблона сообщения.
 * @param NUMBER - номер сообщения
 * @param LEVEL  - уровень сообщения
 * @param TYPE   - тип сообщения
 * @param TEXT   - текст сообщения
 */
#define MESSAGE_TEMPLATE(NUMBER, LEVEL, TYPE, TEXT) \
    const MessageLogTemplate Message##NUMBER = {NUMBER, LEVEL, LOG_SUBSYSTEM, TYPE, TEXT};

/**
 * Макрос для протоколирования идентификаторов
 * @param GUID - идентификатор
 */
#define GUIDLOG(GUID) GUID.toString().c_str();
/**
 * Макрос для протоколирования строкового значения
 * @param TEXT - строковое (QString) значение
 */
#define STRLOG(TEXT) TEXT.c_str()
/**
 * Макрос для протоколирования двух строковых значений
 * @param TEXT1 - первое строковое (QString) значение
 * @param TEXT2 - второе строковое (QString) значение
 */
#define STRLOG2(TEXT1, TEXT2) STRLOG(TEXT1), STRLOG(TEXT2)
/**
 * Макрос для протоколирования трех строковых значений
 * @param TEXT1 - первое строковое (QString) значение
 * @param TEXT2 - второе строковое (QString) значение
 * @param TEXT3 - третье строковое (QString) значение
 */
#define STRLOG3(TEXT1, TEXT2, TEXT3) STRLOG(TEXT1), STRLOG(TEXT2), STRLOG(TEXT3)
/**
 * Макрос для протоколирования четырех строковых значений
 * @param TEXT1 - первое строковое (QString) значение
 * @param TEXT2 - второе строковое (QString) значение
 * @param TEXT3 - третье строковое (QString) значение
 * @param TEXT4 - четвертое строковое (QString) значение
 */
#define STRLOG4(TEXT1, TEXT2, TEXT3, TEXT4) \
    STRLOG(TEXT1), STRLOG(TEXT2), STRLOG(TEXT3), STRLOG(TEXT4)
/**
 * Макрос для протоколирования даты и времени по заданному формату
 * @param DATETIME - значение даты и времени
 * @param FORMAT - форрматирование
 */
#define DTFLOG(DATETIME, FORMAT) DATETIME.toString(FORMAT).toUtf8().constData()
/**
 * Макрос для протоколирования даты и времени
 * @param DATETIME - значение даты и времени
 */
#define DTLOG(DATETIME) DTFLOG(DATETIME, "dd.MM.yyyy HH:mm:ss.zzz")

constexpr int logDetailsDefault =
        LogDetails::Time |
        LogDetails::Type |
        LogDetails::Number |
        LogDetails::Text;

constexpr int logDetailsAllWithoutDate =
        LogDetails::Time |
        LogDetails::Type |
        LogDetails::Number |
        LogDetails::Subsystem |
        LogDetails::ThreadId |
        LogDetails::Text;

constexpr int logDetailsAll =
        LogDetails::Date |
        LogDetails::Time |
        LogDetails::Type |
        LogDetails::Number |
        LogDetails::Subsystem |
        LogDetails::ThreadId |
        LogDetails::Text;

class Logger
{
public:
    explicit Logger(bool doConsoleOutput,
                    const std::string &pathToBin,
                    int level,
                    const int logDetails = LogDetails::Time |
            LogDetails::Type |
            LogDetails::Number |
            LogDetails::Text);
    virtual ~Logger();

    std::string pathToLog() const;
    int level() const;

    bool collectMessage(UnixTimeMSecs created,
                        int number,
                        int level,
                        MessageType messageType,
                        std::string subSystem,
                        THREAD_ID threadId,
                        std::string text);

    bool flush();

    bool snapshot(const std::string &fileName,
                  const std::string &content);

private:
    UnixTimeMSecs currentDate();

    bool prepareFilePath();

    bool openFile();
    void closeFile();

    void printLine(const std::string &line, MessageType messageType);

private:
    bool _doConsoleOutput;
    std::string _pathToLog;
    int _level;

    Descriptor _descriptor;
    std::list<std::string> _buffer;

    std::string _logFilePrefix;
    UnixTimeMSecs _currentLogFileDate;

    int _logDetails;

#ifdef _WIN32
    Descriptor _consoleHandle;
#endif
};

} // namespace Utils
