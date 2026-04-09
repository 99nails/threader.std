#pragma once

#include "LogMessagesTemplates.h"
#include "ListThreads.h"
#include "MessageWriteToFile.h"
#include "SignalHandler.h"
#include "StdStringUtils.h"
#include "ThreadBase.h"

#ifdef linux
#include <sys/resource.h>
#endif

namespace Threader::Threads
{

constexpr uint TIMEOUT_BUILD_STATISTIC_MILLISECONDS = 10000;
constexpr uint TIMEOUT_BUILD_COMMON_STATISTIC_MILLISECONDS = 60000;

class ThreadMainWithouSettings : public ThreadBase
{
public:
    explicit ThreadMainWithouSettings(MessageConsumer *parent = nullptr,
                                      const std::string &applicationBinaryName = std::string(),
                                      const std::string &applicationFriendlyName = std::string(),
                                      const std::string &threadName = std::string());

    const std::string &applicationBinaryName() const;

    const std::string &applicationFriendlyName() const;

protected:
    void onThreadStarted() override;

    void onThreadFinishing() override;

    void onThreadFinished() override;

    void onIdle() override;

private:
    void snapshotThreads(bool rightNow = false);


private:
    std::string _applicationBinaryName;
    std::string _applicationFriendlyName;
    uint64_t _nextBuildStatisticTickCount;
    uint64_t _nextBuildCommonStatisticTickCount;
    std::string _threadStatisticFileName;

};

template <class SettingsSet>
class ThreadMain : public ThreadMainWithouSettings
{
public:
    explicit ThreadMain(MessageConsumer *parent = nullptr,
                        const std::string &applicationBinaryName = std::string(),
                        const std::string &applicationFriendlyName = std::string(),
                        const std::string &threadName = std::string(),
                        const std::string &iniFileName = std::string())
        : ThreadMainWithouSettings(parent,
                                   applicationBinaryName,
                                   applicationFriendlyName,
                                   threadName)
    {
        _settings.setFileName(iniFileName);
    }

    SettingsSet &settings()
    {
        return _settings;
    }
protected:
    void onThreadStarted() override
    {
        ThreadMainWithouSettings::onThreadStarted();

        auto appName = applicationFriendlyName();
        writeLog(Message20, appName.c_str(), _settings.fileName().c_str());
        if (_settings.read())
            writeLog(Message21, appName.c_str(), _settings.fileName().c_str());
        else
            writeLog(Message22, appName.c_str(), _settings.fileName().c_str());
    }

private:
    std::string _iniFileName;
    SettingsSet _settings;
};

}
