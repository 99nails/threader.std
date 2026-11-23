#include <gtest/gtest.h>
#include "UdevDeviceInfo.h"

using namespace Threader::Utils;

TEST(UdevDeviceInfo, ConstructsFromFieldsAndCopies)
{
    PropertyVector props = {{"ACTION", "add"}, {"SUBSYSTEM", "tty"}};
    UdevDeviceInfo info("/dev/ttyUSB0", "/sys/devices/ttyUSB0",
                        "tty", "", "ttyUSB0", "0", "/dev/ttyUSB0", props);

    EXPECT_EQ(info.sysname, "ttyUSB0");
    EXPECT_EQ(info.properties.size(), 2);

    UdevDeviceInfo copy(info);
    EXPECT_EQ(copy.properties, info.properties);
}

