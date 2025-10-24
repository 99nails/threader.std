#include "Logger.h"

#include "FileUtils.h"
#include "StdStringUtils.h"

#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifdef linux
#include <fcntl.h>
//#include <linux/limits.h>
#include <sys/stat.h>
#endif


namespace Threader::Utils {

#ifdef linux
constexpr char ANSI_COLOR_RED[] = "\x1b[31m";
constexpr char ANSI_COLOR_GREEN[] = "\x1b[32m";
constexpr char ANSI_COLOR_YELLOW[] = "\x1b[33m";
//constexpr char ANSI_COLOR_BLUE[] = "\x1b[34m";
//constexpr char ANSI_COLOR_MAGENTA[] = "\x1b[35m";
constexpr char ANSI_COLOR_CYAN[] = "\x1b[36m";
constexpr char ANSI_COLOR_RESET[] = "\x1b[0m";
#endif

Logger::Logger(bool doConsoleOutput,
               const std::string &pathToBin,
               int level,
               int logDetails)
    : _doConsoleOutput(doConsoleOutput)
    , _level(level)
    , _descriptor(INVALID_DESCRIPTOR)
    , _logDetails(logDetails)
{
    auto slashIndex = pathToBin.rfind(FileUtils::DELIMITER);

    _logFilePrefix = (slashIndex == std::string::npos)
            ? pathToBin
            : (slashIndex < pathToBin.length() - 1)
              ? pathToBin.substr(slashIndex + 1)
              : "LogFile";

#ifdef _WIN32
    _consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    if (StdStringUtils::endsWith(StdStringUtils::toUpper(_logFilePrefix), ".EXE"))
    {
        _logFilePrefix.erase(_logFilePrefix.length() - 4, 4);
    }
#endif
    _pathToLog = FileUtils::absolutePath(FileUtils::extractFilePath(pathToBin) +
                                         "/../log/");
    if (!_pathToLog.empty())
    {
        auto end = _pathToLog.back();
        if (end != FileUtils::DELIMITER)
            _pathToLog += FileUtils::DELIMITER;
    }

    prepareFilePath();
}

Logger::~Logger()
{
    closeFile();
}

std::string Logger::pathToLog() const
{
    return _pathToLog;
}

int Logger::level() const
{
    return _level;
}

void Logger::setLevel(const int value)
{
    _level = value;
}

bool Logger::collectMessage(UnixTimeMSecs created,
                            int number,
                            int level,
                            MessageType messageType,
                            std::string subSystem,
                            THREAD_ID threadId,
                            std::string text)
{
    if (level > _level)
        return true;

    auto newCurrentDate = currentDate();
    if (_currentLogFileDate != newCurrentDate)
    {
        closeFile();
        _currentLogFileDate = newCurrentDate;
    }

    if (INVALID_DESCRIPTOR == _descriptor) {
        if (!prepareFilePath())
        {
            return false;
        }

        if (!openFile())
        {
            return false;
        }
    }

    auto appendTab = [](std::string &s, std::string tab = "\t") {
        if (s.empty())
            return;
        s += tab;
    };

    std::string data;

    if (LogDetails::Date & _logDetails) {
        // format = "dd.MM.yyyy";
        data += DateTimeUtils::msecsSinceEpochToStdString(created, false, true,  "%Y-%m-%d");
    }

    if (LogDetails::Time & _logDetails) {
        appendTab(data, " ");
        data += DateTimeUtils::msecsSinceEpochToStdString(created, true, true,  "%H:%M:%S");
    }

    if (!data.empty())
        data = '[' + data + ']';

    if (LogDetails::Type & _logDetails) {
        appendTab(data);
        data += MessagesTypesNames[messageType] + ':';
    }

    if (LogDetails::Number & _logDetails) {
        appendTab(data);
        data += format("%.5d", number);
    }

    if (LogDetails::Subsystem & _logDetails) {
        appendTab(data);
        data += subSystem;
    }

    if (LogDetails::ThreadId & _logDetails) {
        appendTab(data);
        data += std::to_string(threadId);
    }

    appendTab(data);
    data += text + "\r\n";

    _buffer.push_back(data);

    if (_doConsoleOutput)
        printLine(data, messageType);

    return true;
}

bool Logger::flush()
{
    if (INVALID_DESCRIPTOR == _descriptor)
        openFile();

    if (INVALID_DESCRIPTOR == _descriptor)
        return false;

    for (auto iterator = _buffer.cbegin(); iterator != _buffer.cend(); iterator++)
    {
#ifdef linux

        ::write(_descriptor, iterator->c_str(), iterator->length());

#endif
#ifdef _WIN32
        DWORD writtenCount;
        WriteFile(_descriptor, iterator->c_str(), iterator->length(), &writtenCount, nullptr);
#endif
    }
    _buffer.clear();

    return true;
}

bool Logger::snapshot(const std::string &fileName,
                      const std::string &content)
{
    auto descriptor = FileUtils::fileCreate(_pathToLog + fileName);
    if (INVALID_DESCRIPTOR != descriptor)
    {
        bool result = FileUtils::fileWrite(descriptor, content) == content.length();
        FileUtils::fileClose(descriptor);
        return true;
    }
    return false;
}

bool Logger::append(const std::string &fileName,
                    const std::string &content)
{
    auto descriptor = FileUtils::fileOpenOrCreate(_pathToLog + fileName);
    if (INVALID_DESCRIPTOR != descriptor)
    {
        bool result = FileUtils::fileWrite(descriptor, content) == content.length();
        FileUtils::fileClose(descriptor);
        return true;
    }
    return false;
}

UnixTimeMSecs Logger::currentDate()
{
    auto date = DateTimeUtils::msecsSinceEpochLocal();
    date -= date % 86400000;
    return date;
}

bool Logger::prepareFilePath()
{
    if (!FileUtils::isPathExists(_pathToLog))
    {
        auto result = mkdir(_pathToLog.c_str()
                    #ifdef linux
                            , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
                    #endif
                            ) == 0;
        if (!result)
            std::cout << "Ошибка создания каталога " << _pathToLog.c_str() << std::endl;
        return result;

    }
    return true;
}

bool Logger::openFile()
{
    std::string fileName = _pathToLog.c_str() + _logFilePrefix + "-"
                           + DateTimeUtils::msecsSinceEpochToStdString(currentDate(),
                                                                       false,
                                                                       false,
                                                                       "%Y-%m-%d")
                           + ".log";
    _descriptor = FileUtils::fileOpenOrCreate(fileName);
    if (INVALID_DESCRIPTOR == _descriptor)
        return false;

#ifdef linux
    lseek(_descriptor, 0, SEEK_END);
#endif
#ifdef _WIN32
    SetFilePointer(_descriptor, 0, 0, FILE_END);
#endif
    return true;
}

void Logger::closeFile()
{
    flush();

    if (INVALID_DESCRIPTOR == _descriptor)
        return;

    FileUtils::fileClose(_descriptor);
    _descriptor = INVALID_DESCRIPTOR;
}

void Logger::printLine(const std::string &line, MessageType messageType)
{
#ifdef linux
    switch (messageType) {
    case Attention:
        std::cout << ANSI_COLOR_GREEN;
        break;
    case Warning:
        std::cout << ANSI_COLOR_YELLOW;
        break;
    case Error:
        std::cout << ANSI_COLOR_RED;
        break;
    case Debug:
        std::cout << ANSI_COLOR_CYAN;
        break;
    default:
        std::cout << ANSI_COLOR_RESET;
        break;
    }
    std::cout << line << ANSI_COLOR_RESET;
#endif
#ifdef _WIN32
    uint8_t color;
    switch (messageType) {
    case Attention:
        color = 10;
        break;
    case Warning:
        color = 14;
        break;
    case Error:
        color = 12;
        break;
    case Debug:
        color = 11;
        break;
    default:
        color = 7;
    }
    if (INVALID_HANDLE_VALUE != _consoleHandle)
        SetConsoleTextAttribute(_consoleHandle, color);

    std::cout << StdStringUtils::ConvertUTF8ToCP1251(line);

    if (INVALID_HANDLE_VALUE != _consoleHandle)
        SetConsoleTextAttribute(_consoleHandle, 7);
#endif
}

} // namespace Threader::Utils
