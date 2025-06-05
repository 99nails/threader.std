#include "Logger.h"

#include "FileUtils.h"
#include "StdStringUtils.h"

#include <iostream>
#include <unistd.h>

namespace Threader::Utils {

#ifdef linux
constexpr char ANSI_COLOR_RED[] = "\x1b[31m";
constexpr char ANSI_COLOR_GREEN[] = "\x1b[32m";
constexpr char ANSI_COLOR_YELLOW[] = "\x1b[33m";
constexpr char ANSI_COLOR_BLUE[] = "\x1b[34m";
constexpr char ANSI_COLOR_MAGENTA[] = "\x1b[35m";
constexpr char ANSI_COLOR_CYAN[] = "\x1b[36m";
constexpr char ANSI_COLOR_RESET[] = "\x1b[0m";
#endif

Logger::Logger(bool doConsoleOutput, const std::string &pathToBin, int level, int logDetails)
    : _doConsoleOutput(doConsoleOutput)
    , _level(level)
    , _descriptor(INVALID_DESCRIPTOR)
    , _logDetails(logDetails)
{
    auto slashIndex = pathToBin.rfind("/");
    _logFilePrefix = (slashIndex == std::string::npos)
            ? pathToBin
            : (slashIndex < pathToBin.length() - 1)
              ? pathToBin.substr(slashIndex + 1)
              : "LogFile";

    auto path = (slashIndex == 0) ? ""
                                  : pathToBin.substr(0, slashIndex);

    std::filesystem::current_path(path);

    _pathToLog = std::filesystem::absolute("../log/");
//    _logFilePrefix = std::filesystem::path(pathToBin).replace_extension("").filename();

//    std::filesystem::path p = pathToBin;
//    if (p.has_parent_path() && p.parent_path().has_parent_path()) {
//        p = p.parent_path().parent_path();
//    } else
//        p = "..";

//    std::filesystem::path a = p.native() + "/log/";

//    _pathToLog = std::filesystem::absolute(a);

//    _currentLogFileDate = currentDate();

    _pathToLog = "../log/";

    prepareFilePath();
}

Logger::~Logger()
{
    closeFile();
}

int Logger::level() const
{
    return _level;
}

bool Logger::collectMessage(UnuxTimeMSecs created,
                            int number,
                            int level,
                            MessageType messageType,
                            std::string subSystem,
                            Descriptor threadId,
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
            return false;

        if (!openFile())
            return false;
    }

    auto appendTab = [](std::string &s, std::string tab = "\t") {
        if (s.empty())
            return;
        s += tab;
    };

    std::string data;

    if (LogDetails::Date & _logDetails) {
        // format = "dd.MM.yyyy";
        data += DateTimeUtils::msecsSinceEpochToStdString(created, false, true, "%F");
    }

    if (LogDetails::Time & _logDetails) {
        appendTab(data, " ");
        data += DateTimeUtils::msecsSinceEpochToStdString(created, true, true, "%T");
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
        // data += '#' + subSystem + '#';
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
        // std::cout << data.c_str();

    return true;
}

bool Logger::flush()
{
    if (INVALID_DESCRIPTOR == _descriptor)
        return false;

    for (auto iterator = _buffer.cbegin(); iterator != _buffer.cend(); iterator++) {
        ::write(_descriptor, iterator->c_str(), iterator->length());
    }
    _buffer.clear();

    return true;
}

UnuxTimeMSecs Logger::currentDate()
{
    auto date = DateTimeUtils::msecsSinceEpochLocal();
    date -= date % 86400000;
    return date;
}

bool Logger::prepareFilePath()
{
    if (!std::filesystem::exists(_pathToLog))
        return std::filesystem::create_directory(_pathToLog);

    return true;
}

bool Logger::openFile()
{
    std::string fileName = _pathToLog.c_str() + _logFilePrefix + "-"
                           + DateTimeUtils::msecsSinceEpochToStdString(currentDate(),
                                                                       false,
                                                                       false,
                                                                       "%F")
                           + ".log";
    _descriptor = FileUtils::fileOpenOrCreate(fileName);
    if (INVALID_DESCRIPTOR == _descriptor)
        return false;

    lseek(_descriptor, 0, SEEK_END);
    return true;
}

void Logger::closeFile()
{
    flush();

    if (INVALID_DESCRIPTOR == _descriptor)
        return;

    close(_descriptor);
    _descriptor = INVALID_DESCRIPTOR;
}

void Logger::printLine(const std::string &line, MessageType messageType)
{
#ifdef linux
    std::string colorString;
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
    std::cout << line;
#endif
#ifdef _WIN
    uint8_t color;
    switch (message) {
        //    case Message:
        //        color = 8;
        //        break;
    case Attention:
        color = 2;
        break;
    case Warning:
        color = 6;
        break;
    case Error:
        color = 4;
        break;
    case Debug:
        color = 1;
        break;
    default:
        color = 15;
    }
    if (INVALID_HANDLE_VALUE != _consoleHandle)
        SetConsoleTextAttribute(_consoleHandle, color);

    if (_codec) {
        QByteArray dosString = _codec->fromUnicode(messageString.toUtf8());
        std::cout << dosString.constData();
    }
#endif
}

} // namespace Threader::Utils
