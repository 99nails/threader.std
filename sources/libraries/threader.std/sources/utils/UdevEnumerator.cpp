#include "UdevEnumerator.h"

#include "ThreadsCommon.h"

#include <cstring>
#include <iostream>

namespace Threader::Utils
{

UdevEnumerator::UdevEnumerator()
    : _context(nullptr)
{
    initializeContext();
}

UdevEnumerator::~UdevEnumerator()
{
    releaseContext();
}

void UdevEnumerator::enumerate()
{
    // Если контекст не был создан, создаём
    if (!_context)
        initializeContext();

    // выделяем память
    struct udev_enumerate *enumerator = udev_enumerate_new(_context);
    if (enumerator)
    {
        // добавляем фильтр 'subsystem' (tty, usb, scsi_disk, pci_bus, mem, bluetooth, i2c-adapter etc.)
//        udev_enumerate_add_match_subsystem(enumerator, "scsi_disk");

        // сканируем устройства
        udev_enumerate_scan_devices(enumerator);
        // получение указателя на список отсканированных устройств
        struct udev_list_entry *udevList = udev_enumerate_get_list_entry(enumerator);
        struct udev_list_entry *udev_entry;

        udev_list_entry_foreach(udev_entry, udevList)
        {
            const char *name = udev_list_entry_get_name(udev_entry);
//            std::cout << name << std::endl;

            struct udev_device *device = udev_device_new_from_syspath(_context, name);
            const auto listProperties = udev_device_get_properties_list_entry(device);
            struct udev_list_entry *entry;
            // вытаскиваем значения этих свойств
            udev_list_entry_foreach(entry, listProperties) {
                const auto paramName = udev_list_entry_get_name(entry);
                const auto paramValue = udev_device_get_property_value(device, paramName);
//                std::cout << "\t" << paramName
//                          << " = " << paramValue
//                          << std::endl;

                if (strcmp("SUBSYSTEM", paramName) == 0 && _subsystems.count(paramValue) ==  0)
                    _subsystems[paramValue] = "";
            }

        }


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

}
