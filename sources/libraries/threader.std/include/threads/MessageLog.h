
#pragma once

#include "MessageBase.h"
#include "Logger.h"

#include <cstring>

namespace Threader {

namespace Threads {

using namespace Threader::Utils;


class MessageLog : public MessageString
{
public:
    using Ptr = std::shared_ptr<MessageLog>;

public:
    static const std::string MESSAGE_NAME;

    explicit MessageLog(const std::string &text,
                        int number,
                        int level,
                        const std::string &subSystem,
                        MessageType messageType = Message);

    explicit MessageLog(const MessageLogTemplate *messageTemplate, ...);

    explicit MessageLog(const MessageLogTemplate messageTemplate);

    MessageType messageType();
    std::string messageTypeString();
    int number() const;
    int level() const;

    std::string toString(int messageFlags = LogDetails::Time
                                            | LogDetails::Type
                                            | LogDetails::Number
                                            | LogDetails::Subsystem
                                            | LogDetails::Text);

    std::string subSystem() const;

protected:
    int _number;
    int _level;
    MessageType _messageType;
    std::string _subSystem;
};

using VectorMessagesLog = std::vector<MessageLog::Ptr>;

} // namespace Threads
} // namespace Threader
