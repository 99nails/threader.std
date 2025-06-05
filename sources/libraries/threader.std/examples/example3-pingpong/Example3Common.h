#pragma once

#include "Logger.h"

using namespace Threader::Utils;

constexpr char MESSAGE_NAME_SEND_PING[] = "Message.Send.Ping";
constexpr char MESSAGE_NAME_PING[] = "Message.Ping";
constexpr char MESSAGE_NAME_PONG[] = "Message.Pong";

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Main"

MESSAGE_TEMPLATE(10, 0, MessageType::Message,
                 "Запуск потоков...")
MESSAGE_TEMPLATE(11, 0, MessageType::Message,
                 "Все потоки запущены")
MESSAGE_TEMPLATE(12, 0, MessageType::Message,
                 "Завершение потоков...")
MESSAGE_TEMPLATE(13, 0, MessageType::Message,
                 "Все потоки завершены")

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Ping"

MESSAGE_TEMPLATE(20, 0, MessageType::Message,
                 "Поток Ping запущен")
MESSAGE_TEMPLATE(21, 0, MessageType::Message,
                 "Поток Ping завершен")
MESSAGE_TEMPLATE(22, 0, MessageType::Message,
                 "Получено сообщение %s")
MESSAGE_TEMPLATE(23, 0, MessageType::Message,
                 "Отправлено сообщение %s")

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Pong"

MESSAGE_TEMPLATE(30, 0, MessageType::Message,
                 "Поток Pong запущен")
MESSAGE_TEMPLATE(31, 0, MessageType::Message,
                 "Поток Pong завершен")
MESSAGE_TEMPLATE(32, 0, MessageType::Message,
                 "Получено сообщение %s")
MESSAGE_TEMPLATE(33, 0, MessageType::Message,
                 "Отправлено сообщение %s")
