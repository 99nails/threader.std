#pragma once

#include <ThreadFileNotifier.h>

using namespace Threader::Threads;


class ThreadFileWatcher : public ThreadFileNotifier
{
public:
    explicit ThreadFileWatcher(MessageConsumer *parent,
                               const std::string &path,
                               const uint32_t mask = IN_ALL_EVENTS);

    void processFile(const std::string &action,
                     const MaskType mask,
                     const std::string &path,
                     const std::string &fileName) override;

protected:
    void onThreadStarted() override;
    void onThreadFinishing() override;
    void onThreadFinished() override;
};

