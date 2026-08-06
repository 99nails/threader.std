#pragma once

#include "PollerFileNotify.h"
#include "ThreadMain.h"

using namespace Threader::Threads;

class ThreadMainFileWatcher :
        public ThreadMainWithouSettings
{
public:
    explicit ThreadMainFileWatcher(const std::string &applicationBinaryName,
                                      const std::string &applicationFriendlyName);

protected:
    void startChildThreads() override;

private:
    ThreadBase *_threadFileNotifier;

};

