#include "UdevDeviceInfo.h"

namespace Threader::Utils
{

UdevDeviceInfo::UdevDeviceInfo(const std::string &path_,
                               const std::string &syspath_,
                               const std::string &subsystem_,
                               const std::string &devtype_,
                               const std::string &sysname_,
                               const std::string &sysnum_,
                               const std::string &devnode_,
                               const PropertyVector &propertyVector_)
    : path(path_)
    , syspath(syspath_)
    , subsystem(subsystem_)
    , devtype(devtype_)
    , sysname(sysname_)
    , sysnum(sysnum_)
    , devnode(devnode_)
    , properties(propertyVector_)
{
}

UdevDeviceInfo::UdevDeviceInfo(udev_device *device)
{
    // получение параметров устройва
    const char *path = udev_device_get_devpath(device);
    const char *syspath = udev_device_get_syspath(device);
    const char *subsystem = udev_device_get_subsystem(device);
    const char *devtype = udev_device_get_devtype(device);
    const char *sysname = udev_device_get_sysname(device);
    const char *sysnum = udev_device_get_sysnum(device);
    const char *devnode = udev_device_get_devnode(device);

    this->path = path ? path : "";
    this->syspath =syspath ? syspath : "";
    this->subsystem = subsystem ? subsystem : "";
    this->devtype = devtype ? devtype : "";
    this->sysname = sysname ? sysname : "";
    this->sysnum = sysnum ? sysnum : "";
    this->devnode = devnode ? devnode : "";

    // получение списка свойств устройства
    udev_list_entry *propertiesList = udev_device_get_properties_list_entry(device);
    struct udev_list_entry *property_entry;

    PropertyVector propertyVector;

    // проход по списку свойств
    udev_list_entry_foreach(property_entry, propertiesList)
    {
        const char *propertyName = udev_list_entry_get_name(property_entry);
        const char *propertyValue = udev_device_get_property_value(device, propertyName);
        if (propertyName && propertyValue)
        {
            properties.push_back(PropertyEntry(propertyName, propertyValue));
        }
    }
}

UdevDeviceInfo::UdevDeviceInfo(const UdevDeviceInfo &other)
    : path(other.path)
    , syspath(other.syspath)
    , subsystem(other.subsystem)
    , devtype(other.devtype)
    , sysname(other.sysname)
    , sysnum(other.sysnum)
    , devnode(other.devnode)
    , properties(other.properties)
{
}

}
