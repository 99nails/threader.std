#pragma once

#include "PollerUdev.h"
#include "ThreadBase.h"

namespace Threader::Threads {

using namespace Threader::Utils;


class ThreadUdevMonitor : public ThreadBase,
                          public PollerUdevConsumer
{
public:
    explicit ThreadUdevMonitor(MessageConsumer *parent,
                               const std::vector<std::string> &matchedSubsystems = {});

protected:
    void initializeThread() override;

    const std::vector<std::string> &matchedSubsystems() const;

public:
    void processDevice(const std::string &action,
                       const UdevDeviceInfo &deviceInfo) override;
private:
    PollerUdev *_poller;

    std::vector<std::string> _matchedSubsystems;
};


}

