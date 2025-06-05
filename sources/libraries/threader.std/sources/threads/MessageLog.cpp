
#include <stdarg.h>

#include "MessageLog.h"

#include "DateTimeUtils.h"
#include "StdStringUtils.h"

namespace Threader {

namespace Threads {

using namespace Threader::Utils;

const std::string MessageLog::MESSAGE_NAME = "Message.Log";

MessageLog::MessageLog(const std::string &text,
                       int number,
                       int level,
                       const std::string &subSystem,
                       const MessageType messageType)
    : MessageString(MessageLog::MESSAGE_NAME, text)
    , _number(number)
    , _level(level)
    , _messageType(messageType)
    , _subSystem(subSystem)
{
}

MessageLog::MessageLog(const MessageLogTemplate *messageTemplate, ...)
    : MessageString(MessageLog::MESSAGE_NAME, std::string())
    , _number(messageTemplate->number)
    , _level(messageTemplate->level)
    , _messageType(messageTemplate->messageType)
    , _subSystem(messageTemplate->subSystem)
{
    char resultPtr[1024];
    memset(resultPtr, 0, sizeof(resultPtr));

    va_list args;
    va_start(args, messageTemplate);

    auto resultCount = vsnprintf(resultPtr, sizeof(resultPtr),
                                 messageTemplate->text.c_str(),
                                 args);
    va_end(args);

    if (resultCount > 0)
        setValue(std::string(resultPtr));
    else
        setValue(messageTemplate->text);
}

MessageLog::MessageLog(const MessageLogTemplate messageTemplate)
    : MessageString(MessageLog::MESSAGE_NAME, std::string())
    , _number(messageTemplate.number)
    , _level(messageTemplate.level)
    , _messageType(messageTemplate.messageType)
    , _subSystem(messageTemplate.subSystem)
{
    setValue(messageTemplate.text);
}

MessageType MessageLog::messageType()
{
    return _messageType;
}

std::string MessageLog::messageTypeString()
{
    return MessagesTypesNames[_messageType];
}

int MessageLog::number() const
{
    return _number;
}

int MessageLog::level() const
{
    return _level;
}

std::string MessageLog::subSystem() const
{
    return _subSystem;
}

std::string MessageLog::toString(int messageFlags)
{
    // std::string format;
    std::string result = "";
    if (LogDetails::Date & messageFlags) {
        // format = "dd.MM.yyyy";
        result += DateTimeUtils::msecsSinceEpochToStdString(created(),
                                                            false,
                                                            true,
                                                            "%F");
    }

    if (LogDetails::Time & messageFlags) {
        // format = "hh:mm:ss.zzz";
        if (!result.empty())
            result += "\t";
        result += DateTimeUtils::msecsSinceEpochToStdString(created(),
                                                            true,
                                                            true,
                                                            "%T");
    }

    if (!result.empty())
        result = '[' + result + ']';

    if (LogDetails::Type & messageFlags) {
        if (!result.empty())
            result += "\t";
        result += messageTypeString() + ':';
    }

    if (LogDetails::Number & messageFlags) {
        if (!result.empty())
            result += "\t";

        result += format("%.5d", number());
    }

#ifdef USE_LOG_SUBSYSTEM
    if (MessageToStringFlags::Subsystem & messageFlags) {
        if (!result.empty())
            result += "\t";
        result += '#' +subSystem() + '#';
    }
#endif

    if (LogDetails::Text & messageFlags) {
        if (!result.empty())
            result += "\t";
        result += value();
    }

    return result;
}

}}
