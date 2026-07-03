#include "ThreadLogCleaner.h"

#include "LogMessagesTemplates.h"

#include <sys/stat.h>
#include <zlib.h>
#include <fstream>

namespace Threader::Threads
{

constexpr char OPERATION_REMOVE_EXISTING_ARCHIVE[] = "Удаление существующего архива";
constexpr char OPERATION_CREATING_ARCHIVE[] = "Создание архива";
constexpr char OPERATION_REMOVE_FILE[] = "Удаление файла";

ThreadLogCleaner::ThreadLogCleaner(MessageConsumer *parent, const std::string &pathToLog,
                                   const int historyDepth,
                                   const bool pack,
                                   const int packDepth)
    : ThreadBase(parent, "Thread.Log.Cleaner")
    , _pathToLog(pathToLog)
    , _historyDepth(historyDepth)
    , _pack(pack)
    , _packDepth(packDepth)
{
    setTimeout(1000);
}

void ThreadLogCleaner::onThreadStarted()
{
    writeLog(Message4, _historyDepth);
}

void ThreadLogCleaner::onThreadFinishing()
{
}

void ThreadLogCleaner::onThreadFinished()
{
    writeLog(Message5, _historyDepth);
}

void ThreadLogCleaner::onIdle()
{
    struct stat file_stat;
    auto files = FileUtils::scanFiles(_pathToLog);

    constexpr uint secondsByDay = 86400;
    constexpr char EXTENTION_LOG[] = ".log";
    constexpr char EXTENTION_ZIP[] = ".gzip";

    const auto nowSeconds = DateTimeUtils::msecsSinceEpochLocal() / 1000;
    const auto dayStartSeconds = nowSeconds - nowSeconds % secondsByDay;
    const auto packThreshold = dayStartSeconds - _packDepth * secondsByDay;
    const auto deleteThreshold = dayStartSeconds - _historyDepth *secondsByDay;

    for (const auto &file : files)
    {
        auto fileName = _pathToLog + file.d_name;
        if (stat(fileName.c_str(), &file_stat) < 0)
            continue;

        auto extention = FileUtils::extractFileExtension(fileName);

        auto time = file_stat.st_mtime;
        auto timeString = DateTimeUtils::msecsSinceEpochToStdString(time * 1000, true);
        if (timeString.empty())
            continue;

        bool packFile = false;
        bool deleteFile = false;
        // удаление файла
        if (extention == EXTENTION_ZIP && time < deleteThreshold)
        {
            deleteFile = true;
        }

        // упаковка файла
        if (extention == EXTENTION_LOG && time < packThreshold)
        {
            deleteFile = true;
            packFile = true;
        }

        if (packFile)
        {
            auto zipFileName = fileName + EXTENTION_ZIP;
            if (FileUtils::isFileExists(zipFileName))
            {
                logOperationStart(zipFileName, OPERATION_REMOVE_EXISTING_ARCHIVE);
                if (FileUtils::fileDelete(zipFileName))
                    logOperationFinished(zipFileName, OPERATION_REMOVE_EXISTING_ARCHIVE);
                else
                    logOperationError(zipFileName, OPERATION_REMOVE_EXISTING_ARCHIVE, errorString(errorNo()));
            }

            logOperationStart(zipFileName, OPERATION_CREATING_ARCHIVE);
            if (compressFileGzip(fileName, zipFileName))
                logOperationFinished(zipFileName, OPERATION_CREATING_ARCHIVE);
        }

        if (deleteFile)
        {
            logOperationStart(fileName, OPERATION_REMOVE_FILE);
            if (FileUtils::fileDelete(fileName))
                logOperationFinished(fileName, OPERATION_REMOVE_FILE);
            else
                logOperationError(fileName, OPERATION_REMOVE_FILE, errorString(errorNo()));
        }

    }

    terminateThread();
}

void ThreadLogCleaner::logOperationStart(const std::string &fileName,
                                         const std::string &operation)
{
    writeLog(Message6, fileName.c_str(), operation.c_str());
}

void ThreadLogCleaner::logOperationFinished(const std::string &fileName,
                                            const std::string &operation)
{
    writeLog(Message7, fileName.c_str(), operation.c_str());
}

void ThreadLogCleaner::logOperationError(const std::string &fileName,
                                         const std::string &operation,
                                         const std::string &error)
{
    writeLog(Message8, fileName.c_str(), operation.c_str(), error.c_str());
}

bool ThreadLogCleaner::compressFileGzip(const std::string &inputFilename,
                                        const std::string &outputFilename)
{
    auto result = true;
    auto descriptor = FileUtils::fileOpen(inputFilename);
    if (INVALID_DESCRIPTOR == descriptor)
    {
        logOperationError(outputFilename, OPERATION_CREATING_ARCHIVE, errorString(errorNo()));
        return false;
    }

    gzFile zipFile = gzopen(outputFilename.c_str(), "wb9"); // Уровень сжатия 9 (максимум)
    if (!zipFile)
    {
        int errorNo;
        const std::string errorString = gzerror(zipFile, &errorNo);
        logOperationError(outputFilename, OPERATION_CREATING_ARCHIVE, errorString);
        return false;
    }

    constexpr uint32_t BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    uint32_t readCount;
    do
    {
        readCount = ::read(descriptor, buffer, sizeof(buffer));
        if (readCount > 0)
        {
            auto writtenCount = gzwrite(zipFile, buffer, readCount);
            if (0 == writtenCount)
            {
                int errorNo;
                const std::string errorString = gzerror(zipFile, &errorNo);
                logOperationError(outputFilename, OPERATION_CREATING_ARCHIVE, errorString);
                result = false;
                break;
            }
        }
    }
    while (readCount > 0);

    gzclose(zipFile);
    ::close(descriptor);

    return result;
}

}
