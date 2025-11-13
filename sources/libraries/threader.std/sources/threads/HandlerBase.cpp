#include "HandlerBase.h"

#ifdef linux
#include <fcntl.h>
#include <unistd.h>
#endif

namespace Threader::Threads {

HandlerBase::HandlerBase(HandlerDataConsumer *consumer,
                         const std::string &deviceName,
                         const Descriptor descriptor, const bool portionedIO)
#ifdef _WIN32
    : PollerBase()
#endif
#ifdef linux
    : PollerBase(descriptor, 0)
#endif
    , _consumer(consumer)
    , _deviceName(deviceName)
    , _connectionState(ConnectionState::Disconnected)
    , _needsToWrite(false)
    , _portionedIO(portionedIO)
    #ifdef _WIN32
    , _descriptor(descriptor)
    #endif
{
}

#ifdef _WIN32

Descriptor HandlerBase::descriptor() const
{
    return _descriptor;
}

#endif

void HandlerBase::setDescriptor(const Descriptor &value)
{
#ifdef _WIN32
    _descriptor = value;
#endif

#ifdef linux
    PollerBase::setDescriptor(value);
#endif

    if (INVALID_DESCRIPTOR == descriptor())
    {
        setConnectionState(ConnectionState::Disconnected);
    }
    else if (ConnectionState::Connecting != _connectionState)
    {
        setConnectionState(ConnectionState::Connected);
    }
}

HandlerDataConsumer *HandlerBase::consumer() const
{
    return _consumer;
}

bool HandlerBase::setConsumer(HandlerDataConsumer *consumer)
{
    if (INVALID_DESCRIPTOR != descriptor() || _consumer)
        return false;

    _consumer = consumer;
    return true;
}

HandlerBase::ConnectionState HandlerBase::connectionState()
{
    return _connectionState;
}

bool HandlerBase::isConnected()
{
    return ConnectionState::Connected == _connectionState;
}

std::string HandlerBase::deviceName() const
{
    return _deviceName;
}

void HandlerBase::setDeviceName(const std::string &value)
{
    if (ConnectionState::Disconnected != _connectionState)
        close();
    _deviceName = value;
}

bool HandlerBase::needsToWrite() const
{
    return _needsToWrite;
}

void HandlerBase::setNeedsToWrite(const bool value)
{
    _needsToWrite = value;
}

bool HandlerBase::portionedIO()
{
    return _portionedIO;
}

void HandlerBase::setPortionedIO(const bool value)
{
    _portionedIO = value;
}

#ifdef linux

bool isValidDescriptor(int fd)
{
    return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}

int HandlerBase::read(char *data, int count)
{
    if (ConnectionState::Connected == _connectionState && isValidDescriptor(descriptor()))
        return int(::read(descriptor(), data, count));
    return 0;
}

int HandlerBase::write(const void *data, int count)
{
    if (ConnectionState::Connected == _connectionState && isValidDescriptor(descriptor()))
        return int(::write(descriptor(), data, count));
    return 0;
}

#endif

std::string HandlerBase::readString()
{
    std::string result;
    auto bufferSize = inputBytesAvailable();
    if (bufferSize > 0)
    {
        auto data = new char[bufferSize + 1];
        auto readCount = read(data, bufferSize);
        if (readCount)
        {
            data[readCount] = 0;
            result = std::string(data);
        }
        delete[] data;
    }
    return result;
}

int HandlerBase::writeString(const std::string &data)
{
    return int(write(data.c_str(), data.length()));
}

MemoryBlock::Ptr HandlerBase::readBytes()
{
    MemoryBlock::Ptr result = nullptr;

    auto bufferSize = inputBytesAvailable();
    if (bufferSize > 0)
    {
        auto data = new char[bufferSize];
        auto readCount = read(data, bufferSize);
        if (readCount)
        {
            result = std::make_shared<MemoryBlock>(data, readCount);
            // result.push_back(data, readCount);
        }
        delete[] data;
    }
    return result;
}

int HandlerBase::writeBytes(const MemoryBlock::Ptr &data)
{
    return write((char*)data->data(), data->size());
}

void HandlerBase::setConnectionState(HandlerBase::ConnectionState value)
{
    if (ConnectionState::Disconnected == value && descriptor() != INVALID_DESCRIPTOR)
    {
        close();
    }

    if (value != _connectionState)
    {
        auto oldState = _connectionState;
        _connectionState = value;
        if (consumer())
            consumer()->onConnectionStateChanged(this, oldState,  _connectionState);
    }
}

void HandlerBase::doReadyRead()
{
    if (consumer())
        consumer()->onReadyRead(this);
}

void HandlerBase::doReadyWrite()
{
    if (consumer())
        consumer()->onReadyWrite(this);
}

void HandlerBase::doError(int errorCode, std::string errorDescription)
{
    if (consumer())
        consumer()->onError(this, errorCode, errorDescription);
}

}
