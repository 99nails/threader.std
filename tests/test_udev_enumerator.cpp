#include <gtest/gtest.h>
#include "UdevEnumerator.h"

using namespace Threader::Utils;

TEST(UdevEnumerator, EnumeratesNetSubsystem)
{
    UdevEnumerator enumerator({"net"});
    std::vector<UdevDeviceInfo> found;
    UdevEnumerationCallback handler = [&](const UdevDeviceInfo &info) {
        found.push_back(info);
        return true;
    };
    enumerator.enumerate(handler);

    ASSERT_FALSE(found.empty());
    for (const auto &info : found)
        EXPECT_EQ(info.subsystem, "net");
}

// Дополнительно: фиксирование бага с udev_unref(nullptr).
// После исправления releaseContext() тест должен проходить и на машине без udev.
// BUG-4: деструктор должен быть безопасен даже если udev недоступен.
// После исправления releaseContext() (проверка на nullptr, как в PollerUdev)
// тест снимается с DISABLED и обязан проходить на любой машине.
TEST(DISABLED_UdevEnumerator, DestructorIsSafeWhenUdevUnavailable)
{
    for (int i = 0; i < 100; ++i)
    {
        UdevEnumerator enumerator({"net"});
        (void)enumerator.registeredSubsystems();
    }
    SUCCEED();
}
