#pragma once

#include "Logger.h"

using namespace Threader::Utils;

constexpr char MESSAGE_NAME_SEND_PING[] = "Message.Send.Ping";
constexpr char MESSAGE_NAME_PING[] = "Message.Ping";
constexpr char MESSAGE_NAME_PONG[] = "Message.Pong";

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Main"

MESSAGE_TEMPLATE(110, 0, MessageType::Message,
                 "Запуск потоков...")
MESSAGE_TEMPLATE(111, 0, MessageType::Message,
                 "Все потоки запущены")
MESSAGE_TEMPLATE(112, 0, MessageType::Message,
                 "Завершение потоков...")
MESSAGE_TEMPLATE(113, 0, MessageType::Message,
                 "Все потоки завершены")

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Ping"

MESSAGE_TEMPLATE(120, 0, MessageType::Message,
                 "Поток Ping запущен")
MESSAGE_TEMPLATE(121, 0, MessageType::Message,
                 "Поток Ping завершен")
MESSAGE_TEMPLATE(122, 0, MessageType::Message,
                 "Получено сообщение %s")
MESSAGE_TEMPLATE(123, 0, MessageType::Message,
                 "Отправлено сообщение %s")

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Pong"

MESSAGE_TEMPLATE(130, 0, MessageType::Message,
                 "Поток Pong запущен")
MESSAGE_TEMPLATE(131, 0, MessageType::Message,
                 "Поток Pong завершен")
MESSAGE_TEMPLATE(132, 0, MessageType::Message,
                 "Получено сообщение %s")
MESSAGE_TEMPLATE(133, 0, MessageType::Message,
                 "Отправлено сообщение %s")
