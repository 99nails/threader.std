#include "MessageBridge.h"


namespace Threader::Threads::QtEventLoop
{

/***************************************************************************************************
 * MessageBridge
***************************************************************************************************/

MessageBridge::MessageBridge(QObject *parent)
    : _eventBridge(new MessageEventBridge(parent, this))
{
}

MessageBridge::~MessageBridge()
{
    if (_eventBridge)
    {
        delete _eventBridge;
        _eventBridge = nullptr;
    }
}

void MessageBridge::postMessage(const MessageBase::Ptr &message)
{
    if (!_eventBridge)
        return;

    MessageConsumer::postMessage(message);

    _eventBridge->notifyQueueChanged();
}

void MessageBridge::postTerminateEvent()
{
}

void MessageBridge::postEventChildThreadTerminated()
{
}

/***************************************************************************************************
 * MessageBridgeInternal
***************************************************************************************************/

MessageEventBridge::MessageEventBridge(QObject *parent,
                                       MessageBridge *bridge)
    : QObject(parent)
    , _bridge(bridge)
{
    connect(this, &MessageEventBridge::signalQueueChanged,
            this, &MessageEventBridge::slotQueueChanged, Qt::QueuedConnection);
}

void MessageEventBridge::notifyQueueChanged()
{
    emit signalQueueChanged();
}

void MessageEventBridge::slotQueueChanged()
{
    _bridge->processMessages();
}

}
