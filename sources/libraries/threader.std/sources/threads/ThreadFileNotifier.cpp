#include "ThreadFileNotifier.h"

#include "FileUtils.h"

#include <iostream>

namespace Threader::Threads {

ThreadFileNotifier::ThreadFileNotifier(MessageConsumer *parent,
                                       const std::string &path,
                                       const uint32_t mask)
    : ThreadBase(parent, "Thread.FileNotifier")
    , _poller(nullptr)
    , _path(FileUtils::absolutePath(path))
    , _mask(mask)
{
}

void ThreadFileNotifier::initializeThread()
{
    ThreadBase::initializeThread();

    _poller = new PollerFileNotify(this, _path, _mask);
    _poller->initialize();
    polling()->registerPoller(_poller);
}

void ThreadFileNotifier::processFile(const std::string &action,
                                     const MaskType mask,
                                     const std::string &path,
                                     const std::string &fileName)
{
//    std::cout << action << " " << path << "/" << fileName << std::endl;
    (void)action;
    (void)mask;
    (void)fileName;
    (void)fileName;
}

PollerFileNotify *ThreadFileNotifier::poller() const
{
    return _poller;
}

const std::string &ThreadFileNotifier::path() const
{
    return _path;
}

MaskType ThreadFileNotifier::mask() const
{
    return _mask;
}

}

