#include <gtest/gtest.h>
#include <QCoreApplication>
#include "MessageBridge.h"
#include "MessageBase.h"

using namespace Threader::Threads;
using namespace Threader::Threads::QtEventLoop;

// Тестируемый наследник: реализует processMessage и копит имена сообщений
class TestableMessageBridge : public MessageBridge
{
public:
    using MessageBridge::MessageBridge;

    std::vector<std::string> processedNames;

    int queueCount()
    {
        return queue()->count();
    }

protected:
    void processMessage(const MessageBase::Ptr &message) override
    {
        if (message)
            processedNames.push_back(message->name());
    }
};

TEST(MessageBridge, MessagesProcessedOnlyWhenEventLoopRuns)
{
    int argc = 0;
    QCoreApplication app(argc, nullptr);

    TestableMessageBridge bridge;

    bridge.postMessage1<MessageBase>("m.first");
    bridge.postMessage1<MessageBase>("m.second");

    // очередь наполнена, но обработка ещё не шла:
    // сигнал уходит через Qt::QueuedConnection и сработает только в event loop
    EXPECT_TRUE(bridge.processedNames.empty());

    app.processEvents();

    ASSERT_EQ(bridge.processedNames.size(), 2u);
    EXPECT_EQ(bridge.processedNames[0], "m.first");
    EXPECT_EQ(bridge.processedNames[1], "m.second");

    // после обработки очередь пуста — повторный processEvents ничего не добавит
    app.processEvents();
    EXPECT_EQ(bridge.processedNames.size(), 2u);
}

TEST(MessageBridge, NullMessageIsIgnored)
{
    int argc = 0;
    QCoreApplication app(argc, nullptr);

    TestableMessageBridge bridge;
    bridge.postMessage(nullptr);

    app.processEvents();
    EXPECT_TRUE(bridge.processedNames.empty());
}

TEST(MessageBridge, PostMessageEnqueuesWithoutProcessingUntilEventLoop)
{
    int argc = 0;
    QCoreApplication app(argc, nullptr);

    TestableMessageBridge bridge;
    bridge.postMessage1<MessageBase>("m.test");

    // очередь наполнена, но processMessages вызовется только после
    // QueuedConnection-сигнала — то есть после app.processEvents()
    auto queueCount = bridge.queueCount();
    EXPECT_GT(queueCount, 0);

    app.processEvents();

    // после обработки событие сработало; проверить расход через наследника
    // с переопределённым processMessage, который складывает имена в vector
    queueCount = bridge.queueCount();
    EXPECT_EQ(queueCount, 0);
    EXPECT_EQ(bridge.processedNames[0], "m.test");
}
