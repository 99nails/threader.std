#include "MainWindowMessageBridge.h"
#include "./ui_MainWindowMessageBridge.h"

#include "DateTimeUtils.h"
#include "ThreadUtils.h"

#include "ThreadTimer.h"

MainWindowMessageBridge::MainWindowMessageBridge(QWidget *parent)
    : QMainWindow(parent)
    , MessageBridge(this)
    , ui(new Ui::MainWindowMessageBridge)
{
    ui->setupUi(this);

    _testThread = new ThreadTimer(this, "Test", 1000, true);
    _testThread->startThread();

    ui->plainTextEdit->appendPlainText(QString("This thread id: %1").arg(ThreadUtils::currentThreadId()));
}

MainWindowMessageBridge::~MainWindowMessageBridge()
{
    if (_testThread)
    {
        _testThread->postTerminateEvent();
        while (_testThread->isThreadRunning())
        {
            DateTimeUtils::sleepMilliseconds(10);
        }
        _testThread = nullptr;
    }

    delete ui;
}

void MainWindowMessageBridge::processMessage(const MessageBase::Ptr &message)
{
    if (!message)
        return;

    auto line = QString("Message: %1, Message thread id: %2, This thread id: %3")
            .arg(QString::fromStdString(message->name()))
            .arg(message->threadId())
            .arg(ThreadUtils::currentThreadId());

    ui->plainTextEdit->appendPlainText(line);
}
