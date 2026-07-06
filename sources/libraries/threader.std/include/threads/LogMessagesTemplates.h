#pragma once

#include "Logger.h"

using namespace Threader::Utils;

namespace Threader::Threads
{

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Log"


MESSAGE_TEMPLATE(0, 0, MessageType::Attention,
                 "Запуск подсистемы прокотоколирования. Подробность [%d], хранение [%s]")

MESSAGE_TEMPLATE(1, 0, MessageType::Attention,
                 "Завершение подсистемы протоколирования...")

MESSAGE_TEMPLATE(2, 0, MessageType::Attention,
                 "Подсистема протоколирования завершена")

MESSAGE_TEMPLATE(3, 0, MessageType::Attention,
                 "Подробность протоколирования: %d")

MESSAGE_TEMPLATE(4, 0, MessageType::Attention,
                 "Запуск очистки протоколов работы за %d дней...");

MESSAGE_TEMPLATE(5, 0, MessageType::Attention,
                 "Очистка протоколов работы за %d дней завершена");

MESSAGE_TEMPLATE(6, 0, MessageType::Message,
                 "Очистка протоколов работы, файл [%s], операция [%s]...");

MESSAGE_TEMPLATE(7, 0, MessageType::Attention,
                 "Очистка протоколов работы, файл [%s], операция [%s] выполнена");

MESSAGE_TEMPLATE(8, 0, MessageType::Error,
                 "Ошибка очистки протоколов работы, файл [%s], операция [%s]: %s");

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Daemon"


MESSAGE_TEMPLATE(10, 0, MessageType::Attention,
                 "Запуск главного потока службы [%s]")

MESSAGE_TEMPLATE(11, 0, MessageType::Attention,
                 "Завершение главного потока службы [%s]")

MESSAGE_TEMPLATE(12, 0, MessageType::Attention,
                 "Главный поток службы [%s] завершен")

MESSAGE_TEMPLATE(13, 0, Debug,
                 "Запуск потока [%s]: %d")

MESSAGE_TEMPLATE(14, 0, Debug,
                 "Остановлен поток [%s]: %d")

MESSAGE_TEMPLATE(15, 0, Message,
                 "Статистика выполнения - потоков: %d шт., используемая память: %d Kb.")

MESSAGE_TEMPLATE(16, 0, MessageType::Warning,
                 "Получен сигнал завершения службы: %d")

MESSAGE_TEMPLATE(17, 0, MessageType::Warning,
                 "Служба [%s] уже запущена или была завершена ранее с критической ошибкой. Возможно некорректное выполнение приложения!")


MESSAGE_TEMPLATE(20, 0, MessageType::Message,
                 "Чтение настроек службы [%s] из файла [%s]")

MESSAGE_TEMPLATE(21, 0, MessageType::Message,
                 "Чтение настроек службы [%s] из файла [%s] выполнено")

MESSAGE_TEMPLATE(22, 0, MessageType::Error,
                 "Ошибка чтения настроек службы [%s] из файла [%s]. Используются настройки по умолчанию")


MESSAGE_TEMPLATE(50, 0, MessageType::Message,
                 "Регистрация службы [%s]...")

MESSAGE_TEMPLATE(51, 0, MessageType::Attention,
                 "Регистрация службы [%s] выполнена")

MESSAGE_TEMPLATE(52, 0, MessageType::Error,
                 "Ошибка регистрации службы [%s]: %s")


MESSAGE_TEMPLATE(60, 0, MessageType::Message,
                 "Снятие регистрации службы [%s]...")

MESSAGE_TEMPLATE(61, 0, MessageType::Attention,
                 "Снятие регистрации службы [%s] выполнено")

MESSAGE_TEMPLATE(62, 0, MessageType::Error,
                 "Ошибка снятия регистрации службы [%s]: %s")



#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Error"


MESSAGE_TEMPLATE(99999, 0, MessageType::Error, "%s: %d - %s")


#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Common"

}
