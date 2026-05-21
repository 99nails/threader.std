#include "ThreadHandler.h"

#include "MessageBinary.h"

#include "DateTimeUtils.h"

#include <iostream>

namespace Threader {

namespace Threads {

const std::string ThreadHandler::MESSAGE_NAME_DEVICE_CONNECTING = "Device.Connecting";
const std::string ThreadHandler::MESSAGE_NAME_DEVICE_CONNECTED = "Device.Connected";
const std::string ThreadHandler::MESSAGE_NAME_DEVICE_DISCONNECTED = "Device.Disconnected";
const std::string ThreadHandler::MESSAGE_NAME_DEVICE_ERROR = "Device.Error";
const std::string ThreadHandler::MESSAGE_NAME_DEVICE_DATA_INPUT = "Device.Data.Input";
const std::string ThreadHandler::MESSAGE_NAME_DEVICE_DATA_OUTPUT = "Device.Data.Output";

ThreadHandler::ThreadHandler(MessageConsumer *parent,
                             HandlerBase *handler,
                             const uint32_t &reconnectTimeout,
                             PacketFactoryBase *packetFactory,
                             const std::string &threadName)
    : ThreadBase(parent, threadName)
    , _reconnectTimeout(reconnectTimeout)
    , _nextTryToConnect(DateTimeUtils::getTickCount())
    , _packetFactory(packetFactory)
    , _lastPacketSent(DateTimeUtils::getTickCount())
    , _lastPacketReceived(_lastPacketSent)
    , _connectedTickCount(0)
    , _openDateTime(DateTimeUtils::msecsSinceEpochLocal())
    , _closedDateTime(_openDateTime)
{
    // установка обработчика
    setHandler(handler);
    // создание буферов
    _inputBuffer = std::make_shared<MemoryBlock>();
    _outputBuffer = std::make_shared<MemoryBlock>();
}

ThreadHandler::~ThreadHandler()
{
    if (_handler)
    {
        delete _handler;
        _handler = nullptr;
    }

    if (_packetFactory)
    {
        delete _packetFactory;
        _packetFactory = nullptr;
    }
}

void ThreadHandler::initializeThread()
{
    ThreadBase::initializeThread();

    if (!_handler)
        return;

    if (_handler->descriptor() != INVALID_DESCRIPTOR
            && _handler->connectionState() == HandlerBase::ConnectionState::Disconnected)
    {
        _handler->setDescriptor(_handler->descriptor());
    }
    else if (_handler->connectionState() != HandlerBase::ConnectionState::Disconnected)
    {
        onConnectionStateChanged(*_handler,
                                 HandlerBase::ConnectionState::Disconnected,
                                 _handler->connectionState());
    }
}

HandlerBase *ThreadHandler::handler()
{
    return _handler;
}

void ThreadHandler::setHandler(HandlerBase *handler)
{
    _handler = handler;
    if (_handler)
    {
        if (!_handler->consumer())
                _handler->setConsumer(this);

        polling()->registerPoller(_handler);
    }
}

uint32_t ThreadHandler::reconnectTimeout()
{
    return _reconnectTimeout;
}

uint64_t ThreadHandler::nextTryToConnect()
{
    return _nextTryToConnect;
}

PacketFactoryBase *ThreadHandler::packetFactory()
{
    return _packetFactory;
}

void ThreadHandler::clearBuffers()
{
    _inputBuffer->clear();
    _outputBuffer->clear();
}

void ThreadHandler::terminateChildThreads()
{
    ThreadBase::terminateChildThreads();
    if (_handler->isConnected())
        _handler->close();
}

void ThreadHandler::onBeforeWaitEvents()
{
    if (!_handler)
        return;

    // проверка необходимости открытия устройства
    if (_reconnectTimeout > 0 && _handler->connectionState() == HandlerBase::ConnectionState::Disconnected
            && _nextTryToConnect <= DateTimeUtils::getTickCount())
    {
        _handler->open();
    }

    // если идет подключение или подключен
    if (_handler->connectionState() != HandlerBase::ConnectionState::Disconnected)
    {
        // проверка необходимости записи в устройство
        _handler->setNeedsToWrite(_outputBuffer->size() > 0);
    }
}

void ThreadHandler::onConnecting(const HandlerBase &handler)
{
    (void)handler;
}

void ThreadHandler::onConnected(const HandlerBase &handler)
{
    (void)handler;
}

void ThreadHandler::onDisconnected(const HandlerBase &handler)
{
    (void)handler;
}

void ThreadHandler::onReadData(const HandlerBase &handler,
                               const void *data,
                               const int &size)
{
    (void)data;
    (void)size;
}

void ThreadHandler::onWriteData(const HandlerBase &handler,
                                const void *data,
                                const int &size)
{
    (void)data;
    (void)size;
}

bool ThreadHandler::sendPacket(const PacketBase::Ptr &packet)
{
    if (!packet || !_handler->isConnected())
        return false;

    bool result = packet->write(_outputBuffer);
    onReadyWrite(*handler());

    _lastPacketSent = DateTimeUtils::getTickCount();
    onPacketSent(packet);
    return result;
}

uint64_t ThreadHandler::lastPacketSent() const
{
    return _lastPacketSent;
}

uint64_t ThreadHandler::lastPacketReceived() const
{
    return _lastPacketReceived;
}

TrafficCounter *ThreadHandler::inputTrafficCounter()
{
    return &_inputTrafficCounter;
}

TrafficCounter *ThreadHandler::outputTrafficCounter()
{
    return &_outputTrafficCounter;
}

uint64_t ThreadHandler::connectedTickCount() const
{
    return _connectedTickCount;
}

uint64_t ThreadHandler::openDateTime() const
{
    return _openDateTime;
}

uint64_t ThreadHandler::closedDateTime() const
{
    return _closedDateTime;
}

void ThreadHandler::onConnectionStateChanged(HandlerBase &handler,
                                             HandlerBase::ConnectionState oldState,
                                             HandlerBase::ConnectionState newState)
{
    clearBuffers();

    switch (newState) {
    case HandlerBase::ConnectionState::Connecting:
        _openDateTime = DateTimeUtils::msecsSinceEpochLocal();
        _connectedTickCount = 0;

        onConnecting(handler);
        break;
    case HandlerBase::ConnectionState::Connected:
        _lastPacketSent = DateTimeUtils::getTickCount();
        _lastPacketReceived = _lastPacketSent;

        _inputTrafficCounter.reset();
        _outputTrafficCounter.reset();

        _connectedTickCount = DateTimeUtils::getTickCount();
        _openDateTime = DateTimeUtils::msecsSinceEpochLocal();
        _closedDateTime = 0;

        onConnected(handler);
        break;
    case HandlerBase::ConnectionState::Disconnected:
        // вызов метода отключения только в случае, если подключение было установлено
        if (HandlerBase::ConnectionState::Connected == oldState)
        {
            _closedDateTime = DateTimeUtils::msecsSinceEpochLocal();
            onDisconnected(handler);
        }

        _nextTryToConnect = DateTimeUtils::getNextTickCount(_reconnectTimeout);
        _connectedTickCount = 0;
        break;
    }
}

void ThreadHandler::onReadyRead(HandlerBase &sender)
{
    if (sender.connectionState() == HandlerBase::ConnectionState::Disconnected)
        return;

    // чтение данных из входного буфера
    char buffer[65536];
    int readCount;
    uint32_t totalReadCount = 0;

    // MemoryBlock _localInputBuffer;
    // _localInputBuffer.setCapacity(1024);
    _localInputBuffer.clear(false);

    do
    {
        // иницализация шага цикла чтения
        memset(buffer, 0, sizeof(buffer));

        // чтение данных
        readCount = sender.read(buffer, sizeof(buffer));

        if (readCount > 0) {
            // если обмен данными происходит фиксированными порциями как в UDP
            if (sender.portionedIO())
            {
                onReadData(sender, buffer, readCount);
            }
            else
            {
                // сохранение прочитанных данных при успешном чтении
                _localInputBuffer.append(buffer, readCount);
                totalReadCount += uint32_t(readCount);
            }
        }
    } while (readCount > 0);

    // подсчет трафика
    _inputTrafficCounter.append(DateTimeUtils::msecsSinceEpochLocal(),
                                totalReadCount);
    // добавление во входящий буфер
    _inputBuffer->append(_localInputBuffer.data(), _localInputBuffer.size());

    // если обмен данными не происходит фиксированными порциями как в UDP
    if (!sender.portionedIO())
    {
        // вызов заглушки события приема данных
        onReadData(sender, _localInputBuffer.data(), _localInputBuffer.size());

        // если фабрика пакетов не определена
        if (!_packetFactory)
        {
            if (parentThread())
            {
                // создание и отправка сообщения с бинарными данными
                parentThread()->postMessage1<MessageBinary>(MESSAGE_NAME_DEVICE_DATA_INPUT,
                                                            _inputBuffer->data(),
                                                            _inputBuffer->size());
            }
            _inputBuffer->clear();
        }
        else
        {
            // попытка распознавания пакетов с данными, пока они распознаются
            while(true)
            {
                auto packet = _packetFactory->tryExtractPacket(_inputBuffer);
                // если пакет распознан
                if (packet)
                {
                    _lastPacketReceived = DateTimeUtils::getTickCount();
                    onPacketReceived(packet);
                }
                else
                {
//                    int result = _packetFactory->lastResult();
                    // QByteArray code(1, char(result & 0xFF));
                    // auto message(std::make_shared<MessageBinary>(MESSAGE_NAME_DEVICE_ERROR, code));
                    // postMessage(message);
                    break;
                }
            }
       }
    }
}

void ThreadHandler::onReadyWrite(HandlerBase &sender)
{
    // идем лесом если нечего отправить в устройство
    if (_outputBuffer->size() == 0)
        return;

    // непосредственно отправка данных
    int written = sender.write((char*)_outputBuffer->data(), _outputBuffer->size());

    // если хоть что-то ушло в устройство
    if (written > 0) {
        // подсчет отправленных данных
        _outputTrafficCounter.append(DateTimeUtils::msecsSinceEpochLocal(),
                                     written);
        // вызов заглушки протоколирования отправки
        onWriteData(sender, _outputBuffer->data(), written);

        // удаление отправленных данных из буфера
        _outputBuffer->remove(0, written);
    }
}

}}
