#pragma once

#ifdef linux

#include "PollingPosix.h"
#include "ThreadsCommon.h"
#include "UdevEnumerator.h"

#include <libudev.h>

#include <vector>

namespace Threader::Threads
{

using namespace Threader::Utils;


class PollerUdevConsumer
{
public:
    virtual void processDevice(const std::string &action,
                               const DeviceInfo &deviceInfo) = 0;
};

class PollerUdev : public PollerBase
{
public:
    explicit PollerUdev(PollerUdevConsumer *consumer,
                        const std::vector<std::string> &matchedSubsystems = {});
    virtual ~PollerUdev();

    const std::vector<std::string> &matchedSubsystems() const;
    void setMatchedSubsystems(const std::vector<std::string> &matchedSubsystems);

    bool initialize();
    void finalize();

    bool process(const pollfd &event);
private:
    bool initializeContext();
    void releaseContext();

    bool initializeMonitor();
    void releaseMonitor();

private:
    std::vector<std::string> _matchedSubsystems;

    struct udev *_context;
    struct udev_monitor *_monitor;

    PollerUdevConsumer *_consumer;
};

}

#endif
