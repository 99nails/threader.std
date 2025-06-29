#pragma once

#include "Logger.h"
#include "MessageBase.h"
#include "ThreadBase.h"

using namespace Threader::Threads;
using namespace Threader::Utils;

using MessageThread = MessageValue<ThreadBase*>;

constexpr char MESSAGE_NAME_THREAD[] = "Message.Thread";

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Main"

MESSAGE_TEMPLATE(10, 0, MessageType::Message,
                 "Запуск главного потока приложения...")
MESSAGE_TEMPLATE(11, 0, MessageType::Message,
                 "Завершение главного потока приложения...")
MESSAGE_TEMPLATE(12, 0, MessageType::Message,
                 "Главный поток приложения завершен")

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

MESSAGE_TEMPLATE(201, 0, MessageType::Message,
                 "Завершение потока работы с подключением [%s]...")

MESSAGE_TEMPLATE(202, 0, MessageType::Message,
                 "Поток работы с подключением [%s] завершен")
