#pragma once

#include "UdevListener.h"

#include <QMainWindow>

using namespace Threader::Threads::QtEventLoop;

namespace Ui {
class MainWindowUdevListener;
}


class MainWindowUdevListener : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowUdevListener(QWidget *parent = nullptr);
    virtual ~MainWindowUdevListener();

private slots:
    void slotToolButtonRunClicked();

    void slotOnNotify(const Threader::Utils::UdevDeviceInfoList &discoveredDevices,
                  const UdevListener::Notification notification);

private:
    Ui::MainWindowUdevListener *ui;
    UdevListener *_udevListener;

};

