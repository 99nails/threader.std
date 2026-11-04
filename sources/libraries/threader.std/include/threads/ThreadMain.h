#pragma once

#include "LogMessagesTemplates.h"
#include "ThreadBase.h"

#ifdef linux
#include <sys/resource.h>
#endif

namespace Threader::Threads
{

constexpr uint32_t TIMEOUT_BUILD_STATISTIC_MILLISECONDS = 10000;
constexpr uint32_t TIMEOUT_BUILD_COMMON_STATISTIC_MILLISECONDS = 60000;

class ThreadMainWithoutSettings : public ThreadBase
{
public:
    explicit ThreadMainWithoutSettings(MessageConsumer *parent = nullptr,
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
class ThreadMain : public ThreadMainWithoutSettings
{
public:
    explicit ThreadMain(MessageConsumer *parent = nullptr,
                        const std::string &applicationBinaryName = std::string(),
                        const std::string &applicationFriendlyName = std::string(),
                        const std::string &threadName = std::string(),
                        const std::string &iniFileName = std::string())
        : ThreadMainWithoutSettings(parent,
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
        ThreadMainWithoutSettings::onThreadStarted();

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
