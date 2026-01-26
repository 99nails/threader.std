#pragma once

#include "ThreadMain.h"

using namespace Threader::Threads;

class ThreadMainUdpSender : public ThreadMainWithouSettings
{
public:
    explicit ThreadMainUdpSender(const std::string &applicationBinaryName = std::string(),
                                 const std::string &applicationFriendlyName = std::string());

protected:
    void startChildThreads() override;

private:
    ThreadBase *_threadReceiver;
};

