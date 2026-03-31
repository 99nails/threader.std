#pragma once

#include <libudev.h>

#include <list>
#include <string>
#include <vector>

namespace Threader::Utils
{

using PropertyEntry = std::pair<std::string, std::string>;
using PropertyVector = std::vector<PropertyEntry>;

constexpr char UDEV_SUBSYSTEM_GPIO[] = "gpio";
constexpr char UDEV_SUBSYSTEM_NET[] = "net";
constexpr char UDEV_SUBSYSTEM_TTY[] = "tty";
constexpr char UDEV_SUBSYSTEM_WLAN[] = "net/wl";

constexpr char UDEV_ACTION_DEVICE_ADD[] = "add";
constexpr char UDEV_ACTION_DEVICE_REMOVE[] = "remove";

typedef struct UdevDeviceInfo
{
    UdevDeviceInfo(const std::string &path_,
                   const std::string &syspath_,
                   const std::string &subsystem_,
                   const std::string &devtype_,
                   const std::string &sysname_,
                   const std::string &sysnum_,
                   const std::string &devnode_,
                   const PropertyVector &propertyVector_ = {});

    UdevDeviceInfo(udev_device* device);

    UdevDeviceInfo(const UdevDeviceInfo &other);

    std::string path;
    std::string syspath;
    std::string subsystem;
    std::string devtype;
    std::string sysname;
    std::string sysnum;
    std::string devnode;
    PropertyVector properties;
} UdevDeviceInfo;

using UdevDeviceInfoVector = std::vector<UdevDeviceInfo>;
using UdevDeviceInfoList = std::list<UdevDeviceInfo>;

}
