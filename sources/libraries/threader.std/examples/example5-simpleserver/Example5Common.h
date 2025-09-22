#pragma once

#include "Logger.h"
#include "MessageBase.h"
#include "ThreadBase.h"

using namespace Threader::Threads;
using namespace Threader::Utils;

using MessageThread = MessageValue<ThreadBase*>;

constexpr char MESSAGE_NAME_THREAD[] = "Message.Thread";

constexpr char MESSAGE_NAME_DATETIME[] = "Message.DateTime";

constexpr char GPS_PREFIX[] = "$";
constexpr char GPS_PREFIX_GNZDA[] = "GNZDA";
constexpr char GPS_DELIMITER = ',';
constexpr char FORMAT_TEMPLATE_DATETIME[] = "$%s,%.2d%.2d%.2d.000,%.2d,%.2d,%.4d,00,00*FF";

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Main"

MESSAGE_TEMPLATE(10, 0, MessageType::Message,
                 "Запуск главного потока приложения [%s]...")

MESSAGE_TEMPLATE(11, 0, MessageType::Message,
                 "Завершение главного потока приложения [%s]...")

MESSAGE_TEMPLATE(12, 0, MessageType::Message,
                 "Главный поток приложения [%s] завершен")

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
#define LOG_SUBSYSTEM "GPS"

MESSAGE_TEMPLATE(300, 0, MessageType::Message,
                 "Запуск потока эмуляции GPS на порт [%s]...")

MESSAGE_TEMPLATE(301, 0, MessageType::Message,
                 "Завершение потока эмуляции GPS на порт [%s]...")

MESSAGE_TEMPLATE(302, 0, MessageType::Message,
                 "Поток эмуляции GPS на порт [%s] завершен")


MESSAGE_TEMPLATE(310, 0, MessageType::Message,
                 "Подключение к устройству [%s]...")

MESSAGE_TEMPLATE(311, 0, MessageType::Attention,
                 "Подключение к устройству [%s] установлено")

MESSAGE_TEMPLATE(312, 0, MessageType::Message,
                 "Ошибка подключения к устройству [%s] - %s")

MESSAGE_TEMPLATE(313, 0, MessageType::Attention,
                 "Подключение к устройству [%s] закрыто")


MESSAGE_TEMPLATE(314, 0, MessageType::Message,
                 "Получено [%d] байт от подключения [%s]")

MESSAGE_TEMPLATE(315, 0, MessageType::Message,
                 "Передано [%d] байт на подключение [%s]")



MESSAGE_TEMPLATE(316, 0, MessageType::Message,
                 "Получен пакет [%s] байт от подключения [%s]")

MESSAGE_TEMPLATE(317, 0, MessageType::Message,
                 "Передан пакет [%s] байт от подключения [%s]")


MESSAGE_TEMPLATE(320, 0, MessageType::Message,
                 "Получено сообщение [%s]")

MESSAGE_TEMPLATE(321, 0, MessageType::Message,
                 "Отправлено сообщение [%s]")


MESSAGE_TEMPLATE(400, 0, MessageType::Message,
                 "Запуск потока работы с GPS на порт [%s]...")

MESSAGE_TEMPLATE(401, 0, MessageType::Message,
                 "Завершение потока работы с GPS на порт [%s]...")

MESSAGE_TEMPLATE(402, 0, MessageType::Message,
                 "Поток работы с GPS на порт [%s] завершен")


MESSAGE_TEMPLATE(410, 0, MessageType::Message,
                 "Подключение к устройству GPS [%s]...")

MESSAGE_TEMPLATE(411, 0, MessageType::Attention,
                 "Подключение к устройству GPS [%s] установлено")

MESSAGE_TEMPLATE(412, 0, MessageType::Message,
                 "Ошибка подключения к устройству GPS [%s] - %s")

MESSAGE_TEMPLATE(413, 0, MessageType::Attention,
                 "Подключение к устройству GPS [%s] закрыто")


MESSAGE_TEMPLATE(414, 0, MessageType::Message,
                 "Получено [%d] байт от подключения [%s]")

MESSAGE_TEMPLATE(415, 0, MessageType::Message,
                 "Передано [%d] байт на подключение [%s]")


MESSAGE_TEMPLATE(416, 0, MessageType::Message,
                 "Получен пакет [%s] байт от подключения [%s]")

MESSAGE_TEMPLATE(417, 0, MessageType::Message,
                 "Передан пакет [%s] байт от подключения [%s]")


MESSAGE_TEMPLATE(420, 0, MessageType::Message,
                 "Получено сообщение [%s]")

MESSAGE_TEMPLATE(421, 0, MessageType::Message,
                 "Отправлено сообщение [%s]")


MESSAGE_TEMPLATE(430, 0, MessageType::Attention,
                 "Получено время от устройства GPS [%s]")

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Client"

MESSAGE_TEMPLATE(500, 0, MessageType::Message,
                 "Запуск потока работы с подключением [%s]...")

MESSAGE_TEMPLATE(501, 0, MessageType::Message,
                 "Завершение потока работы с подключением [%s]...")

MESSAGE_TEMPLATE(502, 0, MessageType::Message,
                 "Поток работы с подключением [%s] завершен")


MESSAGE_TEMPLATE(509, 0, MessageType::Attention,
                 "Установка подключения к [%s]...")

MESSAGE_TEMPLATE(510, 0, MessageType::Attention,
                 "Подключение к [%s] установлено")

MESSAGE_TEMPLATE(511, 0, MessageType::Attention,
                 "Подключение к [%s] закрыто")

MESSAGE_TEMPLATE(512, 0, MessageType::Message,
                 "Получено [%d] байт от подключения к [%s]")

MESSAGE_TEMPLATE(513, 0, MessageType::Message,
                 "Передано [%d] байт на подключение к [%s]")

MESSAGE_TEMPLATE(514, 0, MessageType::Message,
                 "Получен пакет [%s] от подключения к [%s]")

MESSAGE_TEMPLATE(515, 0, MessageType::Message,
                 "Передан пакет [%s] на подключение к [%s]")

MESSAGE_TEMPLATE(520, 0, MessageType::Error,
                 "Ошибка работы с подключением к [%s] - [%d] %s")

MESSAGE_TEMPLATE(530, 0, MessageType::Attention,
                 "Получено время от сервера [%s]")
