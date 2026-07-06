#pragma once

#include "ThreadsCommon.h"

#include <ctime>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace Threader {

namespace Threads {

class MessageBase
{
public:
    using Ptr = std::shared_ptr<MessageBase>;

public:
    explicit MessageBase(const std::string &name);
    explicit MessageBase(const char name[] = "");
    virtual ~MessageBase() = default;

    virtual std::string toString();

    std::string name() const;
    uint64_t created() const;
    THREAD_ID threadId() const;

protected:
    void initialize();

    void setName(const std::string &name);

private:
    std::string _name;
    uint64_t _created;
    THREAD_ID _threadId;
};


using MessageList = std::list<MessageBase::Ptr>;


using MessageVector = std::vector<MessageBase::Ptr>;


template<class ValueType>
class MessageValue : public MessageBase
{
public:
    using Ptr = std::shared_ptr<MessageValue<ValueType>>;

public:
    explicit MessageValue(const std::string &name,
                          const ValueType &value)
        : MessageBase(name)
        , _value(value)
    {}
    virtual ~MessageValue() = default;

    ValueType value() const
    {
        return _value;
    }

    void setValue(ValueType value)
    {
        if (_value != value)
            _value = value;
    }

private:
    ValueType _value;
};


template<class ValueType, const char* name_ptr>
class MessageValueNamed : public MessageValue<ValueType>
{
public:
    using Ptr = std::shared_ptr<MessageValueNamed<ValueType, name_ptr>>;

    constexpr static auto MESSAGE_NAME = name_ptr;
public:
    explicit MessageValueNamed(const ValueType &value)
        : MessageValue<ValueType>(MESSAGE_NAME, value)
    {
    }
};

#define DECLARE_MESSAGE_NAMED(NAME, NAME_STRING, BASETYPE) \
    constexpr char NAMED_MESSAGE_NAME_##NAME[] = NAME_STRING; \
    class Message##NAME : public MessageValueNamed<BASETYPE, NAMED_MESSAGE_NAME_##NAME> \
{ \
    public: \
    using Ptr = std::shared_ptr<MessageValueNamed<BASETYPE, NAMED_MESSAGE_NAME_##NAME>>; \
    explicit Message##NAME(const BASETYPE &value) \
    : MessageValueNamed<BASETYPE, NAMED_MESSAGE_NAME_##NAME>(value) {} \
};

template<class ReferenceType>
class MessageReference : public MessageBase
{
public:
    using Ptr = std::shared_ptr<MessageValue<ReferenceType>>;

public:
    explicit MessageReference(const std::string &name,
                              const ReferenceType *value,
                              const bool ownsReference = true)
        : MessageBase(name)
        , _reference(_reference)
        , _ownsReference(ownsReference)
    {}

    virtual ~MessageReference()
    {
        if (_ownsReference)
            delete _reference;
    }

    ReferenceType *reference() const { return _reference; }

    bool ownsReference() const { return _ownsReference; }

private:
    ReferenceType *_reference;
    bool _ownsReference;
};

using MessageBoolean = MessageValue<bool>;

using MessageString = MessageValue<std::string>;

using MessageInt = MessageValue<int32_t>;

using MessageUInt = MessageValue<uint32_t>;

using MessageFloat = MessageValue<float>;

using MessageDouble = MessageValue<double>;

} // namespace Threads
} // namespace Threader
