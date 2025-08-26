#pragma once

#include "LogMessagesTemplates.h"
#include "ThreadBase.h"

namespace Threader::Threads
{

template <class SettingsSet>
class ThreadMain : public ThreadBase
{
public:
    explicit ThreadMain(MessageConsumer *parent = nullptr,
                        const std::string &threadName = std::string(),
                        const std::string &iniFileName = std::string(),
                        const ThreadRunMode &threadRunMode = ThreadRunMode::Polling)
        : ThreadBase(parent, threadName, threadRunMode)
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
        writeLog(Message10, threadName().c_str());

        writeLog(Message20, threadName().c_str(), _settings.fileName().c_str());
        if (_settings.read())
            writeLog(Message21, threadName().c_str(), _settings.fileName().c_str());
        else
            writeLog(Message22, threadName().c_str(), _settings.fileName().c_str());
    }

    void onThreadFinishing() override
    {
        writeLog(Message11, threadName().c_str());
    }

    void onThreadFinished() override
    {
        writeLog(Message12, threadName().c_str());
    }

private:
    std::string _iniFileName;
    SettingsSet _settings;
};

}
