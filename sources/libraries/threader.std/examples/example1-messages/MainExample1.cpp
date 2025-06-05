#include "MessageBase.h"

#include "MessageConsumer.h"
#include "ThreadUtils.h"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

using namespace Threader::Threads;

class MessageConsumerThread : public MessageConsumer
{
public:
    MessageConsumerThread()
    {
        _isRunning.store(false);
    }

    void start()
    {
        if (_thread)
            return;

        auto threadFunction = [this](){
            _isRunning.store(true);
            _isTerminated.store(false);

            std::cout << "Thread started. threadId = " << Threader::Utils::ThreadUtils::currentThreadId() << std::endl;

            _count = 0;
            execute();

            _isRunning.store(false);

            std::cout << "Thread finished" << std::endl;
        };

        _thread = new std::thread(threadFunction);
        _thread->detach();
    }

    void stop()
    {
        std::cout << "Thread finishing..." << std::endl;

        if (!_thread || !_isRunning.load())
            return;

        _isTerminated.store(true);

        while (_isRunning.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        std::cout << "Thread finishing completed" << std::endl;
    }

protected:

    void processMessage(const MessageBase::Ptr &message) override
    {
        if (!message)
            return;

        _count++;

        const auto messageName = message->name();
        std::cout << _count << ". " << message->toString() << std::endl;
    }

    void execute()
    {
        while (!_isTerminated.load())
        {
            processMessages();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        processMessages();
    }

private:
    std::thread *_thread = nullptr;
    std::atomic<bool> _isRunning;
    std::atomic<bool> _isTerminated;
    int _count;

    // MessageConsumer interface
public:
    void postTerminateEvent() override {}
    void postEventChildThreadTerminated() override {}
};

int main (int argc, char *argv[])
{
    MessageConsumerThread thread;
    thread.start();

    for (int i = 0; i < 8192; i++)
        thread.postMessage(std::make_shared<MessageString>("Message.String",
                                                           "Some text"));

    thread.stop();

    return 0;
}
