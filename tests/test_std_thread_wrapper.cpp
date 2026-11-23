#include <gtest/gtest.h>
#include "StdThreadWrapper.h"
#include <atomic>
#include <chrono>

using namespace Threader::Utils;

class SimpleThread : public StdThreadWrapper
{
public:
    std::atomic<int> executeCount{0};

protected:
    void execute() override
    {
        executeCount++;
    }
};

TEST(StdThreadWrapper, LifecycleStartTerminateFinish)
{
    SimpleThread thread;
    EXPECT_FALSE(thread.wasStarted());
    EXPECT_FALSE(thread.isThreadRunning());
    EXPECT_FALSE(thread.isThreadFinished());

    thread.startThread();
    thread.terminateThread();

    // ожидание завершения без join (поток detach-нутый)
    for (int i = 0; i < 1000 && !thread.isThreadFinished(); i++)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    EXPECT_TRUE(thread.isThreadFinished());
    EXPECT_EQ(thread.executeCount.load(), 1);
}

TEST(StdThreadWrapper, StartSecondTimeIsNoop)
{
    SimpleThread thread;
    thread.startThread();
    thread.terminateThread();

    for (int i = 0; i < 1000 && !thread.isThreadFinished(); i++)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    thread.startThread(); // повторный старт не должен создать второй std::thread
    EXPECT_EQ(thread.executeCount.load(), 1);
}

TEST(StdThreadWrapper, ExecuteWithoutTerminateLoopsUntilTerminate)
{
    // execute() в реальных ThreadBase крутится в цикле runEventLoop —
    // здесь проверяем, что terminateThread снимает флаг
    SimpleThread thread;
    thread.startThread();
    thread.terminateThread();

    for (int i = 0; i < 1000 && !thread.isThreadFinished(); i++)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    EXPECT_TRUE(thread.isThreadTerminated());
}
