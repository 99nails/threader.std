#pragma once

#include "ThreadMain.h"

using namespace Threader::Threads;

class ThreadMainFileWatcher :
        public ThreadMainWithoutSettings
{
public:
    explicit ThreadMainFileWatcher(const std::string &applicationBinaryName,
                                      const std::string &applicationFriendlyName);

protected:
    void startChildThreads() override;

private:
    ThreadBase *_threadFileNotifier;

};

