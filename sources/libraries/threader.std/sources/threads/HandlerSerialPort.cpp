#include "HandlerSerialPort.h"

#include "SerialUtils.h"

#ifdef linux
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <fileapi.h>
#endif

namespace Threader {

namespace Threads {


using namespace Threader::Utils;


HandlerSerialPort::HandlerSerialPort(HandlerDataConsumer *consumer,
                                     const std::string &deviceName)
    : HandlerBase(consumer, deviceName)
    , _enableParity(false)
    , _twoStopBits(false)
    #ifdef _WIN32
    , _useOverlappedIO(false)
    #endif
{
#ifdef linux
//    _baudRate = B1500000;
//    _baudRate = B57600;
//    _baudRate = B9600;
    _baudRate = B115200;
    _dataSize = CS8;
    _enableParity = false;
    _twoStopBits = false;
#elif defined(_WIN32)
    _baudRate = CBR_115200;
    _dataSize = 8;

    memset(&_overlapped, 0, sizeof(_overlapped));
#else
    _dataSize = 8;
#endif
}

bool HandlerSerialPort::open()
{
    this->close();

    setConnectionState(ConnectionState::Connecting);

    Descriptor serialPortDescriptor = SerialUtils::openSerial(deviceName(),
                                                              _baudRate,
                                                              _enableParity,
                                                              _twoStopBits,
                                                              _dataSize);
    bool result = INVALID_DESCRIPTOR != serialPortDescriptor;

    setDescriptor(serialPortDescriptor);

    if (!result)
    {
        if (consumer())
            consumer()->onError(this, errno);
    }
    else
    {
        setConnectionState(ConnectionState::Connected);

#ifdef _WIN32
        initializeEvent();
        registerEvent();
#endif
    }
    return result;
}

void HandlerSerialPort::close()
{
    Descriptor _descriptor = descriptor();
    if (INVALID_DESCRIPTOR != _descriptor)
    {
#ifdef _WIN32
        unregisterEvent();
#endif
        SerialUtils::closeSerial(_descriptor);
        _descriptor = INVALID_DESCRIPTOR;
        setDescriptor(_descriptor);
    }

    if (connectionState() == ConnectionState::Disconnected)
        return;

    setConnectionState(ConnectionState::Disconnected);

    return;
}

int HandlerSerialPort::inputBytesAvailable()
{
    return SerialUtils::inputBytesAvailable(descriptor());
}

int HandlerSerialPort::outputBytesAvailable()
{
    return SerialUtils::outputBytesAvailable(descriptor());
}

#ifdef _WIN32

DescriptorsVector *HandlerSerialPort::events()
{
    // получение списка событий
    auto eventsVector = HandlerBase::events();

    // проверка дискриптора
    if (descriptor() == INVALID_DESCRIPTOR || eventsVector->size() < 1)
        return eventsVector;

    // установка маски ожидаемых событий
    if (!SerialUtils::setWaitMask(descriptor(), needsToWrite()))
    {
        doError(GetLastError());
        return eventsVector;
    }

    // вызов записи по необходимости
    if (needsToWrite())
        doReadyWrite();

    _mask = 0;
    // запуск ожидания события с overlapped структурой
    if (!WaitCommEvent(descriptor(), &_mask, &_overlapped))
    {
        // при завершении с ошибкой
        auto error = GetLastError();
        // если система в режиме ожидания события
        if (ERROR_IO_PENDING == error)
        {
            // привязка к событию
            eventsVector->clear();
            eventsVector->push_back(_overlapped.hEvent);
        }
        else
        {
            // иначе обработка ошибки
            doError(error);
            close();
        }
    }
    else
        // если событие произошло в момент вызова
        processEvent();

    return eventsVector;
}

bool HandlerSerialPort::process(Descriptor eventToProcess)
{
    if (eventToProcess != _overlapped.hEvent)
        return false;

    ResetEvent(eventToProcess);

    processEvent();
    return true;
}

int HandlerSerialPort::read(char *data, int count)
{
    if (INVALID_DESCRIPTOR == descriptor())
        return 0;

    memset(data, 0, count);

    if (!_useOverlappedIO)
    {
        auto bytesAvalable = SerialUtils::inputBytesAvailable(descriptor());
        count = std::min(count, bytesAvalable);
        if (count <= 0)
            return 0;
    }

    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = CreateEvent(nullptr, true, false, nullptr);

    DWORD readCount = 0;
    if (ReadFile(descriptor(), data, count, &readCount, &overlapped))
    {
        CloseHandle(overlapped.hEvent);
        return readCount;
    }

    int error = GetLastError();
    // если не требуется ожидание завершения
    if (ERROR_IO_PENDING != error)
    {
        doError(error);
    }
    else if (!GetOverlappedResult(descriptor(), &overlapped, &readCount, true))
    {
        doError(GetLastError());
    }

    CloseHandle(overlapped.hEvent);

    return readCount;
}

int HandlerSerialPort::write(const char *data, int count)
{
    if (INVALID_DESCRIPTOR == descriptor())
        return 0;

    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = CreateEvent(nullptr, true, false, nullptr);

    DWORD writtenCount = 0;
    if (WriteFile(descriptor(), const_cast<char*>(data), count, &writtenCount, &overlapped))
    {
        CloseHandle(overlapped.hEvent);
        return writtenCount;
    }

    int error = GetLastError();
    // если не требуется ожидание завершения
    if (ERROR_IO_PENDING != error)
    {
        doError(error);
    }
    else if (!GetOverlappedResult(descriptor(), &overlapped, &writtenCount, true))
    {
        doError(GetLastError());
    }

    CloseHandle(overlapped.hEvent);
    return writtenCount;
}

#endif

#ifdef linux

void HandlerSerialPort::assign(pollfd &event)
{
    ConnectionState currentState = connectionState();
    if (currentState != ConnectionState::Disconnected)
    {
        event.events = POLLIN | POLLERR | POLLHUP | POLLNVAL;
        if (needsToWrite())
            event.events |= POLLOUT;

        event.fd = descriptor();
        event.revents = 0;
    }
}


bool HandlerSerialPort::process(const pollfd &event)
{
    int _descriptor = descriptor();
    if (event.fd == _descriptor)
    {
        // если произошла ошибка, то её нужно отработать в первую очередь
        // пока всё ещё живо
        if (event.revents & POLLERR || event.revents & POLLNVAL)
        {
            doError(errno);
            // и закрытие от греха
            close();
        }

        // следующим нужно проверять закрытие удаленной стороной
        if (event.revents & POLLHUP)
        {
            close();
        }

        // только для подключенного порта или для ожидающего подлючение
        ConnectionState currentConnectionState = connectionState();
        if (currentConnectionState != ConnectionState::Disconnected) {

            // если есть что читать
            if (event.revents & POLLIN) {
                doReadyRead();
            }

            // если есть возможность записать
            if (event.revents & POLLOUT)
            {
                if (connectionState() == ConnectionState::Connecting)
                {
                    setConnectionState(ConnectionState::Connected);
                }
                doReadyWrite();
            }
        }
        return true;
    }
    return false;
}
#endif

uint32_t HandlerSerialPort::baudRate() const
{
    return _baudRate;
}

void HandlerSerialPort::setBaudRate(const uint32_t &baudRate)
{
    _baudRate = baudRate;
}

bool HandlerSerialPort::enableParity() const
{
    return _enableParity;
}

void HandlerSerialPort::setEnableParity(bool enableParity)
{
    _enableParity = enableParity;
}

bool HandlerSerialPort::twoStopBits() const
{
    return _twoStopBits;
}

void HandlerSerialPort::setTwoStopBits(bool twoStopBits)
{
    _twoStopBits = twoStopBits;
}

uint32_t HandlerSerialPort::dataSize() const
{
    return _dataSize;
}

void HandlerSerialPort::setDataSize(const uint32_t &dataSize)
{
#ifdef linux
    if (dataSize == CS5 || dataSize == CS6 || dataSize == CS7 || dataSize == CS8)
        _dataSize = dataSize;
#endif
#ifdef _WIN32
    _dataSize = dataSize;
#endif
}

#ifdef _WIN32

bool HandlerSerialPort::useOverlappedIO() const
{
    return _useOverlappedIO;
}

void HandlerSerialPort::setUseOverlappedIO(bool useOverlappedIO)
{
    _useOverlappedIO = useOverlappedIO;
}

void HandlerSerialPort::initializeEvent()
{
    memset(&_overlapped, 0, sizeof(_overlapped));
    _overlapped.hEvent = CreateEvent(nullptr, true, false, nullptr);
}

void HandlerSerialPort::registerEvent()
{
    unregisterEvent();

    assign({_overlapped.hEvent});
}

void HandlerSerialPort::unregisterEvent()
{
    auto eventsArray = events();
    for (auto e : *eventsArray)
    {
        if (descriptor() != e)
            CloseHandle(e);
    }
    assign({});
}

bool HandlerSerialPort::processEvent()
{
    // событие прихода данных
    if (_mask & EV_RXCHAR)
        doReadyRead();

    // Принят символ заданый полем EvtChar структуры DCB использованой для настройки режимов работы порта
    // if (_mask & EV_RXFLAG);

    // Из буфера передачи передан последний символ
    // не работает на некоторых HUAWEI
    if (_mask & EV_TXEMPTY)
        doReadyWrite();

    // Изменение состояния линии CTS
    // if (_mask & EV_CTS);

    // Изменение состояния линии DSR
    // if (_mask & EV_DSR);

    // Изменение состояния линии RLSD (DCD)
    // if (_mask & EV_RLSD);

    // Состояние разрыва приемной линии
    // if (_mask & EV_BREAK);

    // Ошибка обрамления, перебега или четности
    if (_mask & EV_ERR)
        doError(GetLastError());

    // Входящий звонок на модем (сигнал на линии RI порта)
    // if (_mask & EV_RING);

    // Приемный буфер заполнен на 80 процентов
    if (_mask & EV_RX80FULL)
        doReadyRead();

    return true;
}

#endif

}}
