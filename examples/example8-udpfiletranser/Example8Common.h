#pragma once

#include "Logger.h"

using namespace Threader::Utils;


#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Listen"

MESSAGE_TEMPLATE(1000, 0, MessageType::Message,
                 "Запуск потока получения данных [%s]...")

MESSAGE_TEMPLATE(1001, 0, MessageType::Message,
                 "Завершение потока получения данных [%s]...")

MESSAGE_TEMPLATE(1002, 0, MessageType::Message,
                 "Поток получения данных [%s] завершен")


MESSAGE_TEMPLATE(1010, 9, MessageType::Message,
                 "Включение прослушивания [%s]...")

MESSAGE_TEMPLATE(1011, 0, MessageType::Attention,
                 "Включение прослушивания [%s] выполнено")

MESSAGE_TEMPLATE(1012, 0, MessageType::Warning,
                 "Выключение прослушивания [%s] выполнено")

MESSAGE_TEMPLATE(1013, 9, MessageType::Error,
                 "Ошибка включения прослушивания [%s]: %d - %s")

