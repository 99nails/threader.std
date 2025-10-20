#pragma once

#include "MessageBase.h"

namespace Threader::Threads {

class MessageWriteToFile : public MessageString
{
public:
    enum class WriteMode
    {
        Truncate,
        Append
    };

    using Ptr = std::shared_ptr<MessageWriteToFile>;

public:
    static const std::string MESSAGE_NAME;

    explicit MessageWriteToFile(const std::string &fileName,
                                const std::string &data,
                                const WriteMode writeMode = WriteMode::Append);
    std::string fileName() const;

    WriteMode writeMode() const;

private:
    std::string _fileName;
    WriteMode _writeMode;
};

}
