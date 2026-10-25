#pragma once

#include "ThreadMain.h"

using namespace Threader::Threads;

class ThreadMainSslClient : public ThreadMainWithouSettings
{
public:
    explicit ThreadMainSslClient(const std::string &applicationBinaryName,
                                 const std::string &applicationFriendlyName);

protected:
    void startChildThreads() override;

private:
    ThreadBase *_threadClient;
    ThreadBase *_threadClientSsl;
};
