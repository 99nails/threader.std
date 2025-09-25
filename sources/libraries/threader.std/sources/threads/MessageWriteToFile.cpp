#include "MessageWriteToFile.h"

namespace Threader::Threads {

const std::string MessageWriteToFile::MESSAGE_NAME = "Message.Write.To.File";

MessageWriteToFile::MessageWriteToFile(const std::string &fileName,
                                       const std::string &data,
                                       const WriteMode writeMode)
    : MessageString(MESSAGE_NAME, data)
    , _fileName(fileName)
    , _writeMode(writeMode)
{
}

std::string MessageWriteToFile::fileName() const
{
    return _fileName;
}

MessageWriteToFile::WriteMode MessageWriteToFile::writeMode() const
{
    return _writeMode;
}

}
