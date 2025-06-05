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

MESSAGE_TEMPLATE(100, 0, MessageType::Message,
                 "Запуск потока Ping...")

MESSAGE_TEMPLATE(101, 0, MessageType::Message,
                 "Завершение потока Ping...")

MESSAGE_TEMPLATE(102, 0, MessageType::Message,
                 "Поток Ping завершен")


MESSAGE_TEMPLATE(110, 0, MessageType::Attention,
                 "Подключение к устройству [%s]...")

MESSAGE_TEMPLATE(111, 0, MessageType::Message,
                 "Подключение к устройству [%s] установлено")

MESSAGE_TEMPLATE(112, 0, MessageType::Message,
                 "Ошибка подключения к устройству [%s] - %s")

MESSAGE_TEMPLATE(113, 0, MessageType::Message,
                 "Подключение к устройству [%s] закрыто")


MESSAGE_TEMPLATE(114, 0, MessageType::Message,
                 "Получено [%d] байт от подключения [%s]")

MESSAGE_TEMPLATE(115, 0, MessageType::Message,
                 "Передано [%d] байт на подключение [%s]")



MESSAGE_TEMPLATE(116, 0, MessageType::Message,
                 "Получен пакет [%s] байт от подключения [%s]")

MESSAGE_TEMPLATE(117, 0, MessageType::Message,
                 "Передан пакет [%s] байт от подключения [%s]")


MESSAGE_TEMPLATE(120, 0, MessageType::Message,
                 "Получено сообщение %s")

MESSAGE_TEMPLATE(121, 0, MessageType::Message,
                 "Отправлено сообщение %s")

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Pong"

MESSAGE_TEMPLATE(200, 0, MessageType::Message,
                 "Запуск потока Pong...")

MESSAGE_TEMPLATE(201, 0, MessageType::Message,
                 "Завершение потока Pong...")

MESSAGE_TEMPLATE(202, 0, MessageType::Message,
                 "Поток Pong завершен")


MESSAGE_TEMPLATE(210, 0, MessageType::Attention,
                 "Подключение к устройству [%s]...")

MESSAGE_TEMPLATE(211, 0, MessageType::Message,
                 "Подключение к устройству [%s] установлено")

MESSAGE_TEMPLATE(212, 0, MessageType::Message,
                 "Ошибка подключения к устройству [%s] - %s")

MESSAGE_TEMPLATE(213, 0, MessageType::Message,
                 "Подключение к устройству [%s] закрыто")


MESSAGE_TEMPLATE(214, 0, MessageType::Message,
                 "Получено [%d] байт от подключения [%s]")

MESSAGE_TEMPLATE(215, 0, MessageType::Message,
                 "Передано [%d] байт на подключение [%s]")


MESSAGE_TEMPLATE(216, 0, MessageType::Message,
                 "Получен пакет [%s] байт от подключения [%s]")

MESSAGE_TEMPLATE(217, 0, MessageType::Message,
                 "Передан пакет [%s] байт от подключения [%s]")

