#pragma once

#include <ctime>
#include <list>
#include <memory>
#include <vector>
#include <string>

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
    int threadId() const;

protected:
    void initialize();

    void setName(const std::string &name);

private:
    std::string _name;
    uint64_t _created;
    int _threadId;
};


using MessageList = std::list<MessageBase::Ptr>;


using MessageVector = std::vector<MessageBase::Ptr>;


template<class ValueType>
class MessageValue : public MessageBase
{
public:
    using Ptr = std::shared_ptr<MessageValue<ValueType>>;

public:
    explicit MessageValue(const std::string &name, const ValueType &value)
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

using MessageInt = MessageValue<int>;

using MessageUInt = MessageValue<uint>;

using MessageFloat = MessageValue<float>;

using MessageDouble = MessageValue<double>;

} // namespace Threads
} // namespace Threader
