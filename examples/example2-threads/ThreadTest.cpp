#include "ThreadTest.h"

#include <iostream>

ThreadTest::ThreadTest()
    : ThreadBase(nullptr, "Thread.Test")
    , _count(0)
{
    setTimeout(10000);
}

void ThreadTest::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    _count++;

    const auto messageName = message->name();
    std::cout << _count << ". " << message->toString() << std::endl;
}


void ThreadTest::onProcessMessagesStarted(const int32_t count)
{
    std::cout << "Processing " << count << " messages" << std::endl;
}

void ThreadTest::onProcessMessagesFinished()
{
    std::cout << "Processing finished" << std::endl;
}
