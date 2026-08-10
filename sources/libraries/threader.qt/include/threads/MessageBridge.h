#pragma once

#include "MessageConsumer.h"

#include <QObject>

namespace Threader::Threads::QtEventLoop
{

using namespace Threader::Threads;

class MessageEventBridge;

class MessageBridge : public MessageConsumer
{
    friend class MessageEventBridge;
public:
    explicit MessageBridge(QObject *parent = nullptr);
    virtual ~MessageBridge();

    void postMessage(const MessageBase::Ptr &message) override;
    void postTerminateEvent() override;
    void postEventChildThreadTerminated() override;

private:
    MessageEventBridge *_eventBridge;
};


class MessageEventBridge : public QObject
{
    Q_OBJECT
    friend class MessageBridge;
private:
    MessageEventBridge(QObject *parent, MessageBridge *bridge = nullptr);

private:
    void notifyQueueChanged();

private:
    MessageBridge *_bridge;

signals:
    void signalQueueChanged();

private slots:
    void slotQueueChanged();
};


}

