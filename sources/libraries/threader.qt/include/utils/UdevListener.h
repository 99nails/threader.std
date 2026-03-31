#pragma once

#include "ThreadBase.h"
#include "UdevEnumerator.h"

#include <QObject>

namespace Threader::Threads::QtEventLoop
{

class ThreadUdevListener;

class UdevListener : public QObject
{
    Q_OBJECT

    friend class ThreadUdevListener;
public:
    enum Notification : uint8_t
    {
        Enumerated,
        Connected,
        Disconnected
    };

public:
    explicit UdevListener(QObject *parent);
    virtual ~UdevListener();

    bool startListen(const std::vector<std::string> &matchedSubsystems);
    bool stopListen();

private:
    void notify(const UdevDeviceInfoList &discoveredDevices,
                const Notification notification);

signals:
    void onNotify(const Threader::Utils::UdevDeviceInfoList &discoveredDevices,
                  const UdevListener::Notification notification);

private:
    ThreadUdevListener *_thread;
};

}
