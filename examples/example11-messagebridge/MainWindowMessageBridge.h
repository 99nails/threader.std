#pragma once

#include "MessageBridge.h"
#include "ThreadBase.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowMessageBridge; }
QT_END_NAMESPACE

using namespace Threader::Threads;
using namespace Threader::Threads::QtEventLoop;

class MainWindowMessageBridge : public QMainWindow,
        public MessageBridge
{
    Q_OBJECT

public:
    MainWindowMessageBridge(QWidget *parent = nullptr);
    ~MainWindowMessageBridge();

private:
    Ui::MainWindowMessageBridge *ui;
    MessageBridge *_eventBridge;
    ThreadBase *_testThread;

protected:
    void processMessage(const MessageBase::Ptr &message) override;
};
