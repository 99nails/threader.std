#include "UdevListener.h"

#include "ThreadUdevMonitor.h"

namespace Threader::Threads::QtEventLoop
{

using namespace Threader::Threads;

/***************************************************************************************************
 * ThreadUdevListener
 **************************************************************************************************/

class ThreadUdevListener : public ThreadUdevMonitor
{
public:
    explicit ThreadUdevListener(UdevListener *listener,
                                const std::vector<std::string> &matchedSubsystems);

    void processDevice(const std::string &action,
                       const UdevDeviceInfo &deviceInfo) override;

protected:
    void onThreadStarted() override;

private:
    UdevListener *_listener;

};

ThreadUdevListener::ThreadUdevListener(UdevListener *listener,
                                       const std::vector<std::string> &matchedSubsystems)
    : ThreadUdevMonitor(nullptr, matchedSubsystems)
    , _listener(listener)
{
}

void ThreadUdevListener::processDevice(const std::string &action,
                                       const UdevDeviceInfo &deviceInfo)
{
    if (!_listener)
        return;

    if (action == UDEV_ACTION_DEVICE_ADD)
    {
        _listener->notify({deviceInfo},  UdevListener::Notification::Connected);
    }
    else if (action == UDEV_ACTION_DEVICE_REMOVE)
    {
        _listener->notify({deviceInfo},  UdevListener::Notification::Disconnected);
    }
}

void ThreadUdevListener::onThreadStarted()
{

    UdevDeviceInfoList discoveredList;

    // функция обратного вызова при сканировании устройств
    auto handler = [&](UdevDeviceInfo deviceInfo)
    {
        // добавление в список обнаруженных устройств
        discoveredList.push_back(deviceInfo);
        return true;
    };

    // создание обратного вызова для сканирования
    UdevEnumerationCallback callback = handler;
    // создание нумератора
    UdevEnumerator enumerator(matchedSubsystems());
    // перечисление устройств
    enumerator.enumerate(callback);

    if (_listener)
    {
        _listener->notify(discoveredList,
                          UdevListener::Notification::Enumerated);
    }
}


/***************************************************************************************************
 * UdevListener
 **************************************************************************************************/

UdevListener::UdevListener(QObject *parent)
    : QObject(parent)
    , _thread(nullptr)
{
    qRegisterMetaType<Threader::Utils::UdevDeviceInfoList>("Threader::Utils::UdevDeviceInfoList");
    qRegisterMetaType<UdevListener::Notification>("UdevListener::Notification");
}

UdevListener::~UdevListener()
{
    stopListen();
}

bool UdevListener::startListen(const std::vector<std::string> &matchedSubsystems)
{
    if (_thread)
        return false;

    _thread = new ThreadUdevListener(this, matchedSubsystems);
    _thread->startThread();

    return true;
}

bool UdevListener::stopListen()
{
    if (!_thread)
        return false;

    _thread->postTerminateEvent();
    while (!_thread->isThreadFinished())
    {
        usleep(100000);
    }

    _thread = nullptr;

    return true;
}

void UdevListener::notify(const UdevDeviceInfoList &discoveredDevices,
                          const UdevListener::Notification notification)
{
    emit onNotify(discoveredDevices, notification);
}


}
