#pragma once

#include "Logger.h"
#include "MessageBase.h"
#include "PacketFactoryAsciiLines.h"
#include "ThreadBase.h"

using namespace Threader::Utils;

DECLARE_MESSAGE_NAMED(Packet, "Message.Packet", Threader::Threads::PacketAsciiLine::Ptr)
DECLARE_MESSAGE_NAMED(Thread, "Message.Thread", Threader::Threads::ThreadBasePtr)

constexpr uint16_t PORT_TCP = 45796;
constexpr uint16_t PORT_SSL = 45797;

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Listen"

MESSAGE_TEMPLATE(100, 0, MessageType::Message,
                 "Запуск потока ожидания подключений на порт [%s]...")

MESSAGE_TEMPLATE(101, 0, MessageType::Message,
                 "Завершение потока ожидания подключений на порт [%s]...")

MESSAGE_TEMPLATE(102, 0, MessageType::Message,
                 "Потока ожидания подключений на порт [%s] завершен")


MESSAGE_TEMPLATE(110, 0, MessageType::Message,
                 "Подготовка ожидания подключений на порт [%s]...")

MESSAGE_TEMPLATE(111, 0, MessageType::Message,
                 "Подготовка ожидания подключений на порт [%s] выполнена успешно")

MESSAGE_TEMPLATE(112, 0, MessageType::Error,
                 "Ошибка работы с ожиданием подключений на порт [%s] - %s")

MESSAGE_TEMPLATE(120, 0, MessageType::Attention,
                 "Запрос подключения на порт [%d] с адреса [%s]")

MESSAGE_TEMPLATE(121, 0, MessageType::Attention,
                 "Запрос подключения на порт [%d] с адреса [%s] принят")

MESSAGE_TEMPLATE(122, 0, MessageType::Warning,
                 "Запрос подключения на порт [%d] с адреса [%s] ОТКЛОНЕН!")

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Server"

MESSAGE_TEMPLATE(200, 0, MessageType::Message,
                 "Запуск потока работы с подключением [%s]...")

MESSAGE_TEMPLATE(201, 0, MessageType::Warning,
                 "Завершение потока работы с подключением [%s]...")

MESSAGE_TEMPLATE(202, 0, MessageType::Message,
                 "Поток работы с подключением [%s] завершен")


MESSAGE_TEMPLATE(210, 0, MessageType::Attention,
                 "Подключение к [%s] установлено")

MESSAGE_TEMPLATE(211, 0, MessageType::Attention,
                 "Подключение к [%s] закрыто")

MESSAGE_TEMPLATE(212, 0, MessageType::Message,
                 "Получено [%d] байт от подключения к [%s]")

MESSAGE_TEMPLATE(213, 0, MessageType::Message,
                 "Передано [%d] байт на подключение к [%s]")

MESSAGE_TEMPLATE(214, 0, MessageType::Message,
                 "Получен пакет [%s] от подключения к [%s]")

MESSAGE_TEMPLATE(215, 0, MessageType::Message,
                 "Передан пакет [%s] на подключение к [%s]")

MESSAGE_TEMPLATE(220, 0, MessageType::Error,
                 "Ошибка работы с подключением к [%s] - [%d] %s")

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Client"

MESSAGE_TEMPLATE(300, 0, MessageType::Message,
                 "Запуск потока работы с подключением [%s]...")

MESSAGE_TEMPLATE(301, 0, MessageType::Warning,
                 "Завершение потока работы с подключением [%s]...")

MESSAGE_TEMPLATE(302, 0, MessageType::Message,
                 "Поток работы с подключением [%s] завершен")


MESSAGE_TEMPLATE(309, 0, MessageType::Attention,
                 "Подключение к [%s]...")

MESSAGE_TEMPLATE(310, 0, MessageType::Attention,
                 "Подключение к [%s] установлено")

MESSAGE_TEMPLATE(311, 0, MessageType::Attention,
                 "Подключение к [%s] закрыто")

MESSAGE_TEMPLATE(312, 0, MessageType::Message,
                 "Получено [%d] байт от подключения к [%s]")

MESSAGE_TEMPLATE(313, 0, MessageType::Message,
                 "Передано [%d] байт на подключение к [%s]")

MESSAGE_TEMPLATE(314, 0, MessageType::Message,
                 "Получен пакет [%s] от подключения к [%s]")

MESSAGE_TEMPLATE(315, 0, MessageType::Message,
                 "Передан пакет [%s] на подключение к [%s]")

MESSAGE_TEMPLATE(320, 0, MessageType::Error,
                 "Ошибка работы с подключением к [%s] - [%d] %s")

#undef LOG_SUBSYSTEM


