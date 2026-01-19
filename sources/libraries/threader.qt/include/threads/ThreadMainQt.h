#pragma once

#include "LogMessagesTemplates.h"
#include "ListThreads.h"
#include "MessageWriteToFile.h"
#include "SignalHandler.h"
#include "StdStringUtils.h"
#include "ThreadQtEventLoop.h"

#ifdef linux
#include <sys/resource.h>
#endif

#include "ThreadMain.h"

namespace Threader::Threads::QtEventLoop
{

class ThreadMainQt : public ThreadQtEventLoop
{
    Q_OBJECT
public:
    explicit ThreadMainQt(MessageConsumer *parent = nullptr,
                          const std::string &applicationBinaryName = std::string(),
                          const std::string &applicationFriendlyName = std::string(),
                          const std::string &threadName = std::string());

protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;
    void onIdle() override;

private:
    void snapshotThreads(bool rightNow = false);

private:
    // std::string _iniFileName;
    // SettingsSet _settings;
    std::string _applicationBinaryName;
    std::string _applicationFriendlyName;
    uint64_t _nextBuildStatisticTickCount;
    uint64_t _nextBuildCommonStatisticTickCount;
    std::string _threadStatisticFileName;
};

template <class SettingsSet>
class ThreadMainQtSettings : public ThreadMainQt
{
public:
    explicit ThreadMainQtSettings(MessageConsumer *parent = nullptr,
                                  const std::string &applicationBinaryName = std::string(),
                                  const std::string &applicationFriendlyName = std::string(),
                                  const std::string &threadName = std::string(),
                                  const std::string &iniFileName = std::string())
        : ThreadMainQt(parent,
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
        ThreadMainQt::onThreadStarted();

        writeLog(Message20, _applicationFriendlyName.c_str(), _settings.fileName().c_str());
        if (_settings.read())
            writeLog(Message21, _applicationFriendlyName.c_str(), _settings.fileName().c_str());
        else
            writeLog(Message22, _applicationFriendlyName.c_str(), _settings.fileName().c_str());
    }

private:
    std::string _iniFileName;
    SettingsSet _settings;
};


}
