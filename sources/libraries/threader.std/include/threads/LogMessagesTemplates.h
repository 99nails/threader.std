#pragma once

#include "Logger.h"

using namespace Threader::Utils;

namespace Threader::Threads {

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Log"


MESSAGE_TEMPLATE(0, 0, MessageType::Attention,
                 "Запуск подсистемы прокотоколирования. Подробность [%d], хранение [%s]")

MESSAGE_TEMPLATE(1, 0, MessageType::Attention,
                 "Завершение подсистемы протоколирования...")

MESSAGE_TEMPLATE(2, 0, MessageType::Attention,
                 "Подсистема протоколирования завершена")


#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Daemon"


MESSAGE_TEMPLATE(10, 0, MessageType::Attention,
                 "Запуск главного потока службы [%s]")

MESSAGE_TEMPLATE(11, 0, MessageType::Attention,
                 "Завершение главного потока службы [%s]")

MESSAGE_TEMPLATE(12, 0, MessageType::Attention,
                 "Главный поток службы [%s] завершен")

MESSAGE_TEMPLATE(13, 0, Debug,
                 "Идентификатор потока [%s]: %d");

MESSAGE_TEMPLATE(14, 0, Message,
                 "Статистика выполнения - потоков: %d шт., используемая память: %d Kb.");


MESSAGE_TEMPLATE(20, 0, MessageType::Message,
                 "Чтение настроек службы [%s] из файла [%s]")

MESSAGE_TEMPLATE(21, 0, MessageType::Message,
                 "Чтение настроек службы [%s] из файла [%s] выполнено")

MESSAGE_TEMPLATE(22, 0, MessageType::Error,
                 "Ошибка чтения настроек службы [%s] из файла [%s]. Используются настройки по умолчанию")



#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Error"


MESSAGE_TEMPLATE(99999, 0, MessageType::Error, "%s: %d - %s");


#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Common"

}
