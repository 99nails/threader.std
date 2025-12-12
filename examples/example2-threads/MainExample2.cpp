#include "DateTimeUtils.h"
#include "ThreadTest.h"

#include <iostream>

using namespace Threader::Threads;

int main(int argc, char *argv[])
{
#ifdef QT_SUPPORT
    // this example doesn't works with QT_SUPPORT option
    return 1;
#endif

    (void)argc;
    (void)argv;

    std::cout << "Starting thread..." << std::endl;

    auto thread = new ThreadTest();
    thread->start();

    for (int i = 0; i < 8192; i++)
    {
        thread->postMessage(std::make_shared<MessageString>("Message.String",
                                                            "Some text" + std::to_string(i)));

        // if (i % 10 == 0)
        //     DateTimeUtils::sleepMilliseconds(1);
    }

    thread->terminateThread();
    while (!thread->isFinished())
    {
        DateTimeUtils::sleepMilliseconds(100);
    }

    delete thread;
}
