#pragma once

#include "PollerFileNotify.h"
#include "ThreadBase.h"

namespace Threader::Threads {

using namespace Threader::Utils;


class ThreadFileNotifier : public ThreadBase,
                          public PollerFileNotifyConsumer
{
public:
    explicit ThreadFileNotifier(MessageConsumer* parent,
                                const std::string &path,
                                const uint32_t mask = IN_ALL_EVENTS);

protected:
    void initializeThread() override;

    void processFile(const std::string &action,
                     const MaskType mask,
                     const std::string &path,
                     const std::string &fileName) override;

    PollerFileNotify *poller() const;

    const std::string &path() const;

    MaskType mask() const;

private:
    PollerFileNotify *_poller;
    std::string _path;
    MaskType _mask;
};

}
