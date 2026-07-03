#pragma once

#include "ThreadBase.h"
#include "FileUtils.h"

namespace Threader::Threads
{

using namespace Threader::Utils;

class ThreadLogCleaner : public ThreadBase
{
public:
    explicit ThreadLogCleaner(MessageConsumer *parent,
                              const std::string &pathToLog,
                              const int historyDepth = 30,
                              const bool pack = true,
                              const int packDepth = 2);

protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;
    void onIdle() override;

private:
    void logOperationStart(const std::string &fileName,
                           const std::string &operation);

    void logOperationFinished(const std::string &fileName,
                              const std::string &operation);

    void logOperationError(const std::string &fileName,
                           const std::string &operation,
                           const std::string &error);

    bool compressFileGzip(const std::string &inputFilename,
                         const std::string &outputFilename);
private:
    const std::string _pathToLog;
    const int _historyDepth;
    const bool _pack;
    const int _packDepth;

};

}
