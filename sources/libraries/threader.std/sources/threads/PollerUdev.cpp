#include "PollerUdev.h"

namespace Threader::Threads
{

PollerUdev::PollerUdev(PollerUdevConsumer *consumer,
                       const std::vector<std::string> &matchedSubsystems)
    : PollerBase(INVALID_DESCRIPTOR, POLLIN)
    , _matchedSubsystems(matchedSubsystems)
    , _context(nullptr)
    , _monitor(nullptr)
    , _consumer(consumer)
{
}

PollerUdev::~PollerUdev()
{
    finalize();
}

const std::vector<std::string> &PollerUdev::matchedSubsystems() const
{
    return _matchedSubsystems;
}

void PollerUdev::setMatchedSubsystems(const std::vector<std::string> &matchedSubsystems)
{
    _matchedSubsystems = matchedSubsystems;
}

bool PollerUdev::initialize()
{
    if (!initializeContext())
        return false;

    if (!initializeMonitor())
        return false;

    return true;
}

void PollerUdev::finalize()
{
    releaseMonitor();
    releaseContext();
}

bool PollerUdev::process(const pollfd &event)
{
    if (event.fd != descriptor())
        return false;

    auto device = udev_monitor_receive_device(_monitor);

    DeviceInfo deviceInfo(device);

    bool processed = false;
    if (_consumer)
    {
        for (const auto &pair : deviceInfo.properties)
        {
            if (pair.first == "ACTION")
            {
                processed = true;
                const auto actionValue = pair.second;
                _consumer->processDevice(actionValue, deviceInfo);
                break;
            }
        }

        if (!processed)
        {

        }
    }

    if (device)
        udev_device_unref(device);

    return true;
}

bool PollerUdev::initializeContext()
{
    releaseContext();
    _context = udev_new();
    return (_context != nullptr);
}

void PollerUdev::releaseContext()
{
    if (_context)
    {
        udev_unref(_context);
        _context = nullptr;
    }
}

bool PollerUdev::initializeMonitor()
{
    releaseMonitor();

    // создание монитора устройств и подключение к источнику "udev"
    _monitor = udev_monitor_new_from_netlink(_context, "udev");
    if (!_monitor)
        return false;

    int result;
    // установка фильтров по подсистеме устройств
    for (const auto &subsystem : _matchedSubsystems)
    {
        result = udev_monitor_filter_add_match_subsystem_devtype(_monitor,
                                                                 subsystem.c_str(),
                                                                 nullptr);
        if (result < 0)
            return false;
    }

    // включение источника событий
    result = udev_monitor_enable_receiving(_monitor);
    if (result < 0)
        return false;

    // получение дескриптора монитора
    Descriptor descriptor = udev_monitor_get_fd(_monitor);

    // проверка выполнения
    if (descriptor < 0)
        return false;

    setDescriptor(descriptor);
    return true;
}

void PollerUdev::releaseMonitor()
{
    // освобождение ресурсов монитора
    if (_monitor)
    {
        udev_monitor_unref(_monitor);
        _monitor = nullptr;
    }

    setDescriptor(INVALID_DESCRIPTOR);
}

}
