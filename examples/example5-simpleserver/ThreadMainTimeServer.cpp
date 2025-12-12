#include "ThreadMainTimeServer.h"

#include "Example5Common.h"
#include "ThreadGps.h"
#include "ThreadListenConnections.h"

#include "MessageBase.h"

constexpr char APPLICATION_NAME[] = "Time.Server";
constexpr char THREAD_NAME[] = "Thread.Main.Time.Server";

ThreadMainTimeServer::ThreadMainTimeServer(const std::string &applicationName,
                                           const std::string &description)
    : ThreadMain(nullptr,
                 applicationName,
                 description,
                 THREAD_NAME,
                 format("%s%c%s", SETTINGS_FOLDER, FileUtils::DELIMITER, "example5.ini"))
{
}

void ThreadMainTimeServer::startChildThreads()
{
    _threadListen = registerAndStartChildThread(
                new ThreadListenConnections(this,
                                            settings().get<SettingsTimeServer>()));

    _threadGps = registerAndStartChildThread(new ThreadGps(settings().get<SettingsGps>(), this));
}

void ThreadMainTimeServer::onDestroyTerminatedChildThread(ThreadBase *thread)
{
    _threads.remove(thread);
}

void ThreadMainTimeServer::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    auto messageName = message->name();
    if (MESSAGE_NAME_THREAD == messageName)
        doMessageThread(std::dynamic_pointer_cast<MessageThread>(message));
    else if (MESSAGE_NAME_DATETIME == messageName)
        doMessageDateTime(std::dynamic_pointer_cast<MessageString>(message));
}

void ThreadMainTimeServer::doMessageThread(const MessageThread::Ptr &message)
{
    if (!message)
        return;

    registerAndStartChildThread(message->value(), false);

    _threads.push_back(message->value());
}

void ThreadMainTimeServer::doMessageDateTime(const MessageString::Ptr &message)
{
    if (!message)
        return;

    for (auto thread : _threads)
    {
        auto messageBroadcast = std::make_shared<MessageString>(message->name(),
                                                                message->value());
        thread->postMessage(messageBroadcast);
    }
}

