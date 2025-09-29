#include "UdevEnumerator.h"

#include <algorithm>
#include <iostream>

using namespace Threader::Utils;

bool handler(DeviceInfo deviceInfo)
{
    std::cout << "Path: "<< deviceInfo.path.c_str() << std::endl
              << "\tPath value: "<< deviceInfo.syspath.c_str() << std::endl
              << "\tSubsystem: "<< deviceInfo.subsystem.c_str() << std::endl
              << "\tDev type: "<< deviceInfo.devtype.c_str() << std::endl
              << "\tSys name: "<< deviceInfo.sysname.c_str() << std::endl
              << "\tSys num: "<< deviceInfo.sysnum.c_str() << std::endl
              << "\tDev node: "<< deviceInfo.devnode.c_str() << std::endl;

    std::cout << "\tProperties:" << std::endl;
    for (const auto &property : deviceInfo.properties)
    {
        std::cout << "\t\t" << property.first << ": " << property.second << std::endl;
    }

    return true;
}

bool handlerRadioLookup(DeviceInfo deviceInfo)
{
    auto vendor = std::find(deviceInfo.properties.cbegin(),
                            deviceInfo.properties.cend(),
                            PropertyEntry("ID_VENDOR_ID", "10c4"));
    auto product = std::find(deviceInfo.properties.cbegin(),
                            deviceInfo.properties.cend(),
                            PropertyEntry("ID_MODEL_ID", "ea60"));
    auto found = (vendor != deviceInfo.properties.cend())
                  && (product != deviceInfo.properties.cend());

    if (found)
    {
        std::cout << "Azart 181 P1 found!" << std::endl;
    }

    return !found;
}

int main(int argc, char *argv[])
{
     UdevEnumerationCallback callback = handler;
//    UdevEnumerationCallback callback = handlerRadioLookup;

//     auto enumerator = UdevEnumerator({"tty"});

     auto enumerator = UdevEnumerator({"tty"},
                                      {},
                                      {{"ID_VENDOR_ID", "10c4"},
                                       {"ID_MODEL_ID", "ea60"}});

    enumerator.enumerate(callback);

    return 0;
}
