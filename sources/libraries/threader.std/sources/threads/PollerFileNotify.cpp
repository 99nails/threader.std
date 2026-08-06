#include "PollerFileNotify.h"

#include <map>

#include <unistd.h>

namespace Threader::Threads
{

PollerFileNotify::PollerFileNotify(PollerFileNotifyConsumer *consumer,
                                   const std::string &path,
                                   const MaskType mask)
    : PollerBase(INVALID_DESCRIPTOR, POLLIN)
    , _consumer(consumer)
    , _path(path)
    , _mask(mask)
    , _descriptorNotifier(INVALID_DESCRIPTOR)
{

}

PollerFileNotify::~PollerFileNotify()
{
    finalize();
}

PollerFileNotifyConsumer *PollerFileNotify::consumer() const
{
    return _consumer;
}

const std::string &PollerFileNotify::path() const
{
    return _path;
}

bool PollerFileNotify::initialize()
{
    _descriptorNotifier = inotify_init1(IN_NONBLOCK);
    if (_descriptorNotifier == INVALID_DESCRIPTOR)
        return false;

    _descriptorWatch = inotify_add_watch(_descriptorNotifier,
                                         _path.c_str(),
                                         _mask);

    if (_descriptorWatch == INVALID_DESCRIPTOR) {
        close(_descriptorNotifier);
        return false;
    }

    setDescriptor(_descriptorNotifier);

    return true;
}

void PollerFileNotify::finalize()
{
    inotify_rm_watch(_descriptorNotifier, _descriptorWatch);
    close(_descriptorNotifier);
}

bool PollerFileNotify::process(const pollfd &event)
{
    if (event.fd != descriptor())
        return false;

    char buffer[4096];
    ssize_t readCount = read(_descriptorNotifier, buffer, sizeof(buffer));
    if (readCount < 0)
    {
        return false;
    }

    // Обрабатываем каждое событие
    for (char* p = buffer; p < buffer + readCount; )
    {
        inotify_event* notifyEvent = reinterpret_cast<inotify_event*>(p);
        auto eventMask = notifyEvent->mask & IN_ALL_EVENTS;
        switch (eventMask)
        {
        case IN_CREATE:
            break;
        case IN_DELETE:
            break;
        case IN_MODIFY:
            break;
        }
        p += sizeof(inotify_event) + notifyEvent->len;

        auto action = operationName(eventMask);
        // std::cout << action << " " << notifyEvent->name << std::endl;
        if (consumer())
            consumer()->processFile(action, eventMask, _path, notifyEvent->name);
    }
    return true;
}

std::string PollerFileNotify::operationName(const MaskType code)
{
    static const std::map<uint32_t, std::string> operationNames = {
        {IN_ACCESS, "Access"},
        {IN_MODIFY, "Modify"},
        {IN_ATTRIB, "Attributes"},
        {IN_CLOSE_WRITE, "Close write"},
        {IN_CLOSE_NOWRITE, "Close read"},
        {IN_OPEN, "Open"},
        {IN_MOVED_FROM, "Moved from"},
        {IN_MOVED_TO, "Moved to"},
        {IN_CREATE, "Create"},
        {IN_DELETE, "Delete"},
        {IN_DELETE_SELF, "Delete self"},
        {IN_MOVE_SELF, "Move self"}
    };

    if (operationNames.count(code) > 0)
        return operationNames.at(code);
    return std::string();
}

}
