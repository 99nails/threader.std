#include "ThreadFileWatcher.h"

MESSAGE_TEMPLATE(1100, 9, MessageType::Message,
                 "Запуск потока слежения за файлами в каталоге [%s]")

MESSAGE_TEMPLATE(1101, 9, MessageType::Message,
                 "Завершение потока слежения за файлами в каталоге [%s]")

MESSAGE_TEMPLATE(1102, 9, MessageType::Message,
                 "Поток слежения за файлами в каталоге [%s] завершен")

MESSAGE_TEMPLATE(1103, 9, MessageType::Message,
                 "Действие [%s] маска [0x%.8x] над файлом [%s] в каталоге [%s]")


ThreadFileWatcher::ThreadFileWatcher(MessageConsumer* parent,
                                     const std::string &path,
                                     const uint32_t mask)
    : ThreadFileNotifier(parent, path, mask)
{
}


void ThreadFileWatcher::processFile(const std::string &action,
                                    const MaskType mask,
                                    const std::string &path,
                                    const std::string &fileName)
{
    writeLog(Message1103, action.c_str(), mask, fileName.c_str(), path.c_str());
}


void ThreadFileWatcher::onThreadStarted()
{
    writeLog(Message1100, path().c_str());
}

void ThreadFileWatcher::onThreadFinishing()
{
    writeLog(Message1101, path().c_str());
}

void ThreadFileWatcher::onThreadFinished()
{
    writeLog(Message1102, path().c_str());
}
