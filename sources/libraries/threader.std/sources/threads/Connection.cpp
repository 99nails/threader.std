#include "Connection.h"

#include "DateTimeUtils.h"

namespace Threader {

namespace Threads {

ConnectionBase::ConnectionBase(ThreadBase *parentThread,
                               HandlerBase *handler,
                               const uint32_t &reconnectTimeout,
                               PacketFactoryBase* packetFactory,
                               const bool &portionedIO)
    : _parentThread(parentThread)
    , _consumer(dynamic_cast<ConnectionDataConsumer*>(_parentThread))
    , _reconnectTimeout(reconnectTimeout)
    , _nextTryToConnect(DateTimeUtils::getTickCount())
    , _inputBuffer(std::make_shared<MemoryBlock>(65536))
    , _outputBuffer(std::make_shared<MemoryBlock>(65536))
    , _packetFactory(packetFactory)
    , _portionedIO(portionedIO)
    , _connectedTickCount(0)
    , _connectedDateTime(0)
    , _disconnectedDateTime(0)
{
    setHandler(handler);
}

HandlerBase *ConnectionBase::handler() const
{
    return _handler;
}

void ConnectionBase::setHandler(HandlerBase *handler)
{
    // установка обработчика
    _handler = handler;
    // проверка допустимости указателя на обработчик
    if (!_handler)
        return;

    // установка потребителя событий
    _handler->setConsumer(this);

    // при допустимости указателя на поток-владелец
    if (_parentThread)
    {
        // регистрация обработчика в пуле слушателей событий
        _parentThread->polling()->registerPoller(_handler);
    }
}

void ConnectionBase::onBeforeWaitEvents()
{
    if (!_handler)
        return;

    // проверка необходимости открытия устройства
    if (_reconnectTimeout > 0 && _handler->connectionState() == HandlerBase::ConnectionState::Disconnected
            && _nextTryToConnect <= DateTimeUtils::getTickCount())
    {
        if (!_handler->open())
            _nextTryToConnect = DateTimeUtils::getNextTickCount(_reconnectTimeout);
    }

    // если идет подключение или подключен
    if (_handler->connectionState() != HandlerBase::ConnectionState::Disconnected)
    {
        // проверка необходимости записи в устройство
        bool needsToWrite = _outputBuffer->size() > 0;
        _handler->setNeedsToWrite(needsToWrite);
    }
}

void ConnectionBase::sendPacket(const PacketBase::Ptr &packet)
{
    if (!packet || !_handler)
        return;

    if (packet->write(_outputBuffer))
    {
        onReadyWrite(*handler());

        _lastPacketSent = DateTimeUtils::getTickCount();

        if (_consumer)
            _consumer->onPacketSent(*this, packet);
    }
}

void ConnectionBase::sendData(const void *data, const size_t size)
{
    if (!data || size == 0 || !_handler )
        return;

    _outputBuffer->append(data, size);
    onReadyWrite(*handler());
}

void ConnectionBase::sendString(const std::string &data)
{
    if (data.empty() || !_handler)
        return;

    _outputBuffer->append(data.c_str(), data.size());
    onReadyWrite(*handler());
}

uint64_t ConnectionBase::lastPacketSent() const
{
    return _lastPacketSent;
}

uint64_t ConnectionBase::lastPacketReceived() const
{
    return _lastPacketReceived;
}

const TrafficCounter &ConnectionBase::inputTrafficCounter() const
{
    return _inputTrafficCounter;
}

const TrafficCounter &ConnectionBase::outputTrafficCounter() const
{
    return _outputTrafficCounter;
}

uint64_t ConnectionBase::connectedTickCount() const
{
    return _connectedTickCount;
}

const uint64_t &ConnectionBase::connectedDateTime() const
{
    return _connectedDateTime;
}

const uint64_t &ConnectionBase::disconnectedDateTime() const
{
    return _disconnectedDateTime;
}

void ConnectionBase::onReadData(const char *data, const int &size)
{
    if (_consumer)
        _consumer->onReadData(*this, data, size);
}

void ConnectionBase::onWriteData(const char *data, const int &size)
{
    if (_consumer)
        _consumer->onWriteData(*this, data, size);
}

void ConnectionBase::clearBuffers()
{
    _inputBuffer->clear();
    _outputBuffer->clear();
}

PacketFactoryBase *ConnectionBase::packetFactory() const
{
    return _packetFactory;
}

void ConnectionBase::onConnectionStateChanged(HandlerBase &sender,
                                              const HandlerBase::ConnectionState oldState,
                                              const HandlerBase::ConnectionState newState)
{
    if (!_handler || sender != *_handler)
        return;

    clearBuffers();

    switch (newState)
    {
    case HandlerBase::ConnectionState::Disconnected:
        _disconnectedDateTime = DateTimeUtils::msecsSinceEpochLocal();
        _nextTryToConnect = DateTimeUtils::getNextTickCount(_reconnectTimeout);
        break;
    case HandlerBase::ConnectionState::Connecting:
        _connectedTickCount = 0;
        _connectedDateTime = 0;
        _disconnectedDateTime = 0;
        break;
    case HandlerBase::ConnectionState::Connected:
        _lastPacketSent = DateTimeUtils::getTickCount();
        _lastPacketReceived = _lastPacketSent;

        _inputTrafficCounter.reset();
        _outputTrafficCounter.reset();

        _connectedTickCount = DateTimeUtils::getTickCount();
        _connectedDateTime = DateTimeUtils::msecsSinceEpochLocal();
        _disconnectedDateTime = 0;

        break;
    }

    if (_consumer)
        _consumer->onConnectionStateChanged(*this, oldState, newState);
}


void ConnectionBase::onReadyRead(HandlerBase &sender)
{
    if (!_handler || sender != *_handler
            || sender.connectionState() == HandlerBase::ConnectionState::Disconnected)
        return;

    // чтение данных из входного буфера
    char buffer[65536];
    int readCount;
    uint32_t totalReadCount = 0;

    MemoryBlock localInputBuffer;
    localInputBuffer.setCapacity(64 * 1024);

    do
    {
        // иницализация шага цикла чтения
        memset(buffer, 0, sizeof(buffer));

        // чтение данных
        readCount = sender.read(buffer, sizeof(buffer));

        if (readCount > 0) {
            // если обмен данными происходит фиксированными порциями как в UDP
            if (_portionedIO)
            {
                onReadData(buffer, readCount);
            }
            else
            {
                // сохранение прочитанных данных при успешном чтении
                localInputBuffer.append(buffer, readCount);
                totalReadCount += uint32_t(readCount);
            }
        }
    } while (readCount > 0);

    // подсчет трафика
    _inputTrafficCounter.append(DateTimeUtils::msecsSinceEpochLocal(), totalReadCount);
    // добавление во входящий буфер
    _inputBuffer->append(localInputBuffer.data(), localInputBuffer.size());

    // если обмен данными не происходит фиксированными порциями как в UDP
    if (!_portionedIO)
    {
        // вызов заглушки события приема данных
        onReadData((char*)localInputBuffer.data(), localInputBuffer.size());

        // если фабрика пакетов не определена
        if (!_packetFactory)
        {
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
                    if (_consumer)
                        _consumer->onPacketReceived(*this, packet);
                }
                else
                {
//                    int result = _packetFactory->lastResult();
//                    emit signalOnPacketExctractError(this, result);
                    break;
                }
            }
        }
    }
}

void ConnectionBase::onReadyWrite(HandlerBase &sender)
{
    if (!_handler || sender != *_handler)
        return;

    // идем лесом если нечего отправить в устройство
    if (_outputBuffer->size() == 0)
        return;

    // непосредственно отправка данных
    int written = sender.write(_outputBuffer->data(), size_t(_outputBuffer->size()));

    // если хоть что-то ушло в устройство
    if (written > 0) {
        // подсчет отправленных данных
        _outputTrafficCounter.append(DateTimeUtils::msecsSinceEpochLocal(), written);
        // вызов заглушки протоколирования отправки
        onWriteData((char*)_outputBuffer->data(), written);

        // удаление отправленных данных из буфера
        _outputBuffer->remove(0, written);
    }
}

void ConnectionBase::onError(HandlerBase &sender,
                             const int errorCode,
                             const std::string &errorDescription)
{
    if (!_handler || sender != *_handler)
        return;

    //    emit signalOnError(this, errorCode);
}

}}
