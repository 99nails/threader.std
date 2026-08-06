#include "ThreadMainTimeClient.h"

#include "Example05Common.h"
#include "ThreadConnectionClient.h"

#include "MessageBase.h"

constexpr char APPLICATION_NAME[] = "Time.Server";
constexpr char THREAD_NAME[] = "Thread.Main.Time.Client";

ThreadMainTimeClient::ThreadMainTimeClient(const std::string &applicationName,
                                           const std::string &description)
    : ThreadMain(nullptr,
                 applicationName,
                 description,
                 THREAD_NAME,
                 format("%s%c%s", SETTINGS_FOLDER, FileUtils::DELIMITER, "example5.ini"))
{
}

void ThreadMainTimeClient::startChildThreads()
{
    _threadClient = registerAndStartChildThread(new ThreadConnectionClient(this,
                                                                           settings().get<SettingsTimeServer>()));
}

void ThreadMainTimeClient::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    const auto messageName = message->name();
   if (MESSAGE_NAME_THREAD == messageName)
       doMessageThread(std::dynamic_pointer_cast<MessageThread>(message));
   else if (MESSAGE_NAME_DATETIME == messageName)
       doMessageDateTime(std::dynamic_pointer_cast<MessageString>(message));
}

void ThreadMainTimeClient::doMessageThread(const MessageThread::Ptr &message)
{
    if (!message)
        return;

    registerAndStartChildThread(message->value(), false);
}

void ThreadMainTimeClient::doMessageDateTime(const MessageString::Ptr &message)
{
    if (!message)
        return;
}

