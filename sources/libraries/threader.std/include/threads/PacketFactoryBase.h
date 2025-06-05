#pragma once

#include "MemoryBlock.h"

#include <memory>

namespace Threader::Threads {

using namespace Threader::Utils;

enum class extractErrors : int
{
    NoError = 0,
    DataEmpty,
    NoHeader,
    NoFullHeader,
    NeedMoreData,
    CRCError,
    NoPacket,
    BadDirection,
    Error00FF,
    Error0506,
    ErrorFFEE,
    ErrorPacketSize,
    DeviceNotSyncronized,
    DeviceUnsync,
    DeviceSync,
    DeviceSyncAfterReset
};

class PacketBase
{
public:
    using Ptr = std::shared_ptr<PacketBase>;

public:
    explicit PacketBase(const char* data,
                        const int size);
    explicit PacketBase(int size);
    explicit PacketBase(const MemoryBlock::Ptr &data);
    virtual ~PacketBase();

    void clear();

    uint64_t created() const;
    uint64_t createdTickCount() const;

    void *data() const;
    void setData(const void *data, const size_t size);
    void setData(const MemoryBlock::Ptr &data);

    virtual bool write(MemoryBlock::Ptr &data) const = 0;
private:
    uint64_t _createdUtc;
    uint64_t _createdTickCount;
    void* _data;
};

class PacketFactoryBase
{
public:
    explicit PacketFactoryBase() = default;
    virtual ~PacketFactoryBase() = default;
    virtual PacketBase::Ptr tryExtractPacket(MemoryBlock::Ptr &data) = 0;
    virtual PacketBase::Ptr buildPacket(const void *data,
                                        bool generatePacketId) = 0;
    virtual int lastResult() const;

protected:
    void setLastResult(int value);

private:
    int _lastResult;
};

}
