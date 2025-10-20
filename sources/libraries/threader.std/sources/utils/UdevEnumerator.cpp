#include "UdevEnumerator.h"

#include <algorithm>
#include <cstring>
#include <iostream>

namespace Threader::Utils
{

constexpr char PROPERTY_NAME_SUBSYSTEM[] = "SUBSYSTEM";

UdevEnumerator::UdevEnumerator(const std::vector<std::string> &matchedSubsystems,
                               const std::vector<std::string> &unmatchedSubsystems,
                               const PropertyVector &matchedProperties)
    : _context(nullptr)
    , _matchedSubsystems(matchedSubsystems)
    , _unmatchedSubsystems(unmatchedSubsystems)
    , _matchedProperties(matchedProperties)
{
    initializeContext();

    collectSubsystemNames();
}

UdevEnumerator::~UdevEnumerator()
{
    releaseContext();
}

std::vector<std::string> UdevEnumerator::registeredSubsystems() const
{
    return _registeredSubsystems;
}

std::vector<std::string> UdevEnumerator::matchedSubsystems() const
{
    return _matchedSubsystems;
}

void UdevEnumerator::setMatchedSubsystems(const std::vector<std::string> &matchedSubsystems)
{
    _matchedSubsystems = matchedSubsystems;
}

std::vector<std::string> UdevEnumerator::unmatchedSubsystems() const
{
    return _unmatchedSubsystems;
}

void UdevEnumerator::setUnmatchedSubsystems(const std::vector<std::string> &unmatchedSubsystems)
{
    _unmatchedSubsystems = unmatchedSubsystems;
}

PropertyVector UdevEnumerator::matchedProperties() const
{
    return _matchedProperties;
}

void UdevEnumerator::setMatchedProperties(const PropertyVector &matchedProperties)
{
    _matchedProperties = matchedProperties;
}

void UdevEnumerator::enumerate(UdevEnumerationCallback &handler)
{
    // создание контекста при его отсутствии
    if (!_context)
        initializeContext();

    // создание нумератора
    struct udev_enumerate *enumerator = udev_enumerate_new(_context);
    if (enumerator)
    {
        // установка правил совпадения подсистем при перечислении
        for (const auto &subsystem : _matchedSubsystems)
            udev_enumerate_add_match_subsystem(enumerator, subsystem.c_str());

        // установка правил исключения подсистем из перечисления
        for (const auto &subsystem : _unmatchedSubsystems)
                udev_enumerate_add_nomatch_subsystem(enumerator, subsystem.c_str());

        for (const auto &property : _matchedProperties)
            udev_enumerate_add_match_property(enumerator,
                                          property.first.c_str(),
                                          property.second.c_str());

        // сканирование устройств
        udev_enumerate_scan_devices(enumerator);

        // получение указателя на список отсканированных устройств
        struct udev_list_entry *udevList = udev_enumerate_get_list_entry(enumerator);
        struct udev_list_entry *udev_entry;

        // проход по списку отсканированных устройств
        udev_list_entry_foreach(udev_entry, udevList)
        {
            // получение имени устройства
            const char *syspath = udev_list_entry_get_name(udev_entry);

            // получение устройства по имени
            struct udev_device *device = udev_device_new_from_syspath(_context, syspath);

            // получение параметров устройва
            const char *path = udev_device_get_devpath(device);
            const char *subsystem = udev_device_get_subsystem(device);
            const char *devtype = udev_device_get_devtype(device);
            const char *sysname = udev_device_get_sysname(device);
            const char *sysnum = udev_device_get_sysnum(device);
            const char *devnode = udev_device_get_devnode(device);

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
                    propertyVector.push_back(PropertyEntry(propertyName, propertyValue));
                }
            }

            // вызов
            DeviceInfo deviceInfo(syspath ? syspath : "",
                                  path ? path : "",
                                  subsystem ? subsystem : "" ,
                                  devtype ? devtype : "",
                                  sysname ? sysname : "",
                                  sysnum ? sysnum : "",
                                  devnode ? devnode : "",
                                  propertyVector);
            if (!handler(deviceInfo))
                break;
        }
        // освобождение нумератора
        udev_enumerate_unref(enumerator);
    }
}

void UdevEnumerator::initializeContext()
{
    _context = udev_new();
    if (_context == nullptr)
        return;
}

void UdevEnumerator::releaseContext()
{
    udev_unref(_context);
}

void UdevEnumerator::collectSubsystemNames()
{
    // создание контекста при его отсутствии
    if (!_context)
        initializeContext();

    // создание нумератора
    struct udev_enumerate *enumerator = udev_enumerate_new(_context);
    if (enumerator)
    {
        // сканирование устройств
        udev_enumerate_scan_devices(enumerator);
        // получение указателя на список отсканированных устройств
        struct udev_list_entry *udevList = udev_enumerate_get_list_entry(enumerator);
        struct udev_list_entry *udev_entry;

        udev_list_entry_foreach(udev_entry, udevList)
        {
            // получение имени устройства
            const char *name = udev_list_entry_get_name(udev_entry);
            // получение устройства по имени
            struct udev_device *device = udev_device_new_from_syspath(_context, name);
            // получение списка свойств устройства
            const auto listProperties = udev_device_get_properties_list_entry(device);
            // получение значений свойств
            struct udev_list_entry *entry;
            udev_list_entry_foreach(entry, listProperties) {
                // получение имени свойства
                const auto paramName = udev_list_entry_get_name(entry);
                // получение значения свойства
                const auto paramValue = udev_device_get_property_value(device, paramName);
                // если свойство - имя подсистемы
                if (strcmp(PROPERTY_NAME_SUBSYSTEM, paramName) == 0)
                {
                    // если подсистема не найден в списке
                    if (std::find(_registeredSubsystems.cbegin(), _registeredSubsystems.cend(),
                                  std::string(paramValue)) == _registeredSubsystems.cend())
                    {
                        // добавление в список подсистем
                        _registeredSubsystems.push_back(paramValue);
                    }
                }
            }
        }

        // освобождение нумератора
        udev_enumerate_unref(enumerator);
    }
}

}
