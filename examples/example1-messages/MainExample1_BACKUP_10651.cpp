#include "MessageBase.h"

#include "MessageConsumer.h"
#include "ThreadUtils.h"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

using namespace Threader::Threads;

DECLARE_MESSAGE_NAMED(NamedString, "Message.Named.String", std::string)

class MessageConsumerThread : public MessageConsumer
{
public:
    explicit MessageConsumerThread()
        : MessageConsumer()
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

            std::cout << "Thread started. threadId = "
                      << Threader::Utils::ThreadUtils::currentThreadId() << std::endl;

            _count = 0;
            run();

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

    void run()
    {
        while (!_isTerminated.load())
        {
            processMessages();
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
    (void)argc;
    (void)argv;

    MessageConsumerThread thread;
    thread.start();

    for (int i = 0; i < 8192; i++)
    {
<<<<<<< HEAD
//        thread.postMessage(std::make_shared<MessageString>("Message.String",
//                                                           "Some text"));
        thread.postMessage1<MessageNamedString>("Some text");

=======
        thread.postMessage(std::make_shared<MessageString>("Message.String",
                                                           "Some text"));
>>>>>>> 175d8671fa41471cac68ac9c93cdd4b32641390d

        if (i % 1000 == 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    thread.stop();

    return 0;
}
