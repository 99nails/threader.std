#include "MainWindowUdevListener.h"
#include "ui_MainWindowUdevListener.h"

#include "UdevEnumerator.h"

using namespace Threader::Utils;

MainWindowUdevListener::MainWindowUdevListener(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowUdevListener)
{
    ui->setupUi(this);

    _udevListener = new UdevListener(this);

    connect(ui->toolButtonRun, &QToolButton::clicked,
            this, &MainWindowUdevListener::slotToolButtonRunClicked);

    connect(_udevListener, &UdevListener::onNotify,
            this, &MainWindowUdevListener::slotOnNotify, Qt::QueuedConnection);
}

MainWindowUdevListener::~MainWindowUdevListener()
{
    if (_udevListener)
        _udevListener->stopListen();

    delete ui;
}

void MainWindowUdevListener::slotToolButtonRunClicked()
{
    if (!_udevListener)
        return;

    if (ui->toolButtonRun->isChecked())
    {
        // Допустимые значения
        // UDEV_SUBSYSTEM_GPIO, UDEV_SUBSYSTEM_NET,
        // UDEV_SUBSYSTEM_TTY, UDEV_SUBSYSTEM_WLAN
        _udevListener->startListen({UDEV_SUBSYSTEM_TTY, UDEV_SUBSYSTEM_NET});
    }
    else
    {
        _udevListener->stopListen();
    }
}

void MainWindowUdevListener::slotOnNotify(const Threader::Utils::UdevDeviceInfoList &discoveredDevices,
                                          const UdevListener::Notification notification)
{
    switch (notification)
    {
    case UdevListener::Enumerated:
        for (const auto &deviceInfo : discoveredDevices)
        {
            ui->textEditLog->append("Установлено устройство " + QString::fromStdString(deviceInfo.sysname));
        }
        break;
    case UdevListener::Connected:
        for (const auto &deviceInfo : discoveredDevices)
        {
            ui->textEditLog->append("Подключилось устройство " + QString::fromStdString(deviceInfo.sysname));
        }
        break;
    case UdevListener::Disconnected:
        for (const auto &deviceInfo : discoveredDevices)
        {
            ui->textEditLog->append("Отключилось устройство " + QString::fromStdString(deviceInfo.sysname));
        }
        break;
    }
}
