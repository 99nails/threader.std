#pragma once

#include "Logger.h"

using namespace Threader::Utils;


#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Listen"

MESSAGE_TEMPLATE(1000, 0, MessageType::Message,
                 "Запуск потока получения данных...")

MESSAGE_TEMPLATE(1001, 0, MessageType::Message,
                 "Завершение потока получения данных...")

MESSAGE_TEMPLATE(1002, 0, MessageType::Message,
                 "Поток получения данных завершен")


MESSAGE_TEMPLATE(1010, 9, MessageType::Message,
                 "Подключение входящего UDP порта [%s]...")

MESSAGE_TEMPLATE(1011, 0, MessageType::Attention,
                 "Подключение входящего UDP порта [%s] выполнено")

MESSAGE_TEMPLATE(1012, 0, MessageType::Warning,
                 "Отключение входящего UDP порта [%s] выполнено")

MESSAGE_TEMPLATE(1013, 0, MessageType::Error,
                 "Ошибка подключения входящего UDP порта [%s]: %d - %s")


MESSAGE_TEMPLATE(1020, 9, MessageType::Message,
                 "Подключение исходящего UDP порта [%s]...")

MESSAGE_TEMPLATE(1021, 0, MessageType::Attention,
                 "Подключение исходящего UDP порта [%s] выполнено")

MESSAGE_TEMPLATE(1022, 0, MessageType::Warning,
                 "Отключение исходящего UDP порта [%s] выполнено")

MESSAGE_TEMPLATE(1023, 0, MessageType::Error,
                 "Ошибка подключения исходящего UDP порта [%s]: %d - %s")


MESSAGE_TEMPLATE(1030, 9, MessageType::Message,
                 "Получено [%d] байт из входящего порта [%s]")

MESSAGE_TEMPLATE(1031, 9, MessageType::Message,
                 "Передано [%d] байт в исходящий порт [%s]")

