#pragma once

#ifdef linux

#include "PollingPosix.h"
#include "ThreadsCommon.h"
#include "UdevEnumerator.h"

#include <sys/inotify.h>

namespace Threader::Threads
{

using namespace Threader::Utils;

using MaskType = uint32_t;

class PollerFileNotifyConsumer
{
public:
    virtual void processFile(const std::string &action,
                             const MaskType mask,
                             const std::string &path,
                             const std::string &fileName) = 0;
};

class PollerFileNotify : public PollerBase
{
public:
    explicit PollerFileNotify(PollerFileNotifyConsumer *consumer,
                              const std::string &path,
                              const MaskType mask = IN_ALL_EVENTS);
    virtual ~PollerFileNotify();

    PollerFileNotifyConsumer *consumer() const;

    const std::string &path() const;

    bool initialize();
    void finalize();

    bool process(const pollfd &event) override;

    std::string operationName(const MaskType code);
private:
    PollerFileNotifyConsumer *_consumer;
    std::string _path;
    MaskType _mask;
    Descriptor _descriptorNotifier;
    Descriptor _descriptorWatch;
};

}

#endif
