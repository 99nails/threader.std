#include <gtest/gtest.h>
#include "DateTimeUtils.h"
#include "TrafficCounter.h"

using namespace Threader::Utils;

TEST(TrafficCounter, EmptyCounterSpeedIsZero)
{
    TrafficCounter counter;
    EXPECT_EQ(counter.actualize(), 0);
    EXPECT_EQ(counter.total(), 0);
}

TEST(TrafficCounter, AppendAccumulatesTotal)
{
    TrafficCounter counter;
    auto now = DateTimeUtils::msecsSinceEpochLocal();

    counter.append(now, 100);
    counter.append(now + 100, 200);

    EXPECT_EQ(counter.total(), 300);
}

TEST(TrafficCounter, ActualizeDropsEntriesOlderThanWindow)
{
    TrafficCounter counter(1); // окно 1 секунда
    auto now = DateTimeUtils::msecsSinceEpochLocal();

    counter.append(now - 5000, 1000); // старее окна — вытеснится
    counter.append(now, 500);

    EXPECT_EQ(counter.actualize(), 500);
    EXPECT_EQ(counter.total(), 1500); // total не теряет историю
}

TEST(TrafficCounter, SecondsToStoreAffectsWindow)
{
    TrafficCounter counter;
    counter.setSecondsToStore(10);

    auto now = DateTimeUtils::msecsSinceEpochLocal();
    counter.append(now - 5000, 700); // в пределах окна 10 сек

    EXPECT_EQ(counter.actualize(), 70); // 700 / 10
}
