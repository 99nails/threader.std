#include "CommandLineParser.h"

#include <iostream>

using namespace Threader::Utils;

int main(int argc, char *argv[])
{
    CommandLineParser parser(
        {
            CommandLineOption("Вывод протокола работы в терминал", "--console", "-c", false, false),
            CommandLineOption("Установка уровня протоколирования [-1..9]", "--log-level", "-l", true, false),
            CommandLineOption("Задание файла настроек", "--config", "-f", true, false),
        },

        {
            CommandLineCommand("Регистрация службы в systemd", "install",
                               {
                                   CommandLineOption("Идентификатор службы в systemd", "--name", "-n", true, false),
                                   CommandLineOption("Командная строка запуска службы", "--exec-start", "-e", true, false)
                               }),
            CommandLineCommand("Снятие регистрации службы systemd", "uninstall",
                               {
                                   CommandLineOption("Идентификатор службы в systemd", "--name", "-n", true, false)
                               }),
            CommandLineCommand("Запрос конфиграции узла", "configure",
                               {
                                   CommandLineOption("Идентификатор группы", "--group", "-g", true, true),
                                   CommandLineOption("Короткое название", "--short-name", "-s", true, true)
                               }),
            CommandLineCommand("Подключение внешнего узла", "outer-connect",
                               {
                                   CommandLineOption("Идентификатор в системе", "--app-id", "", true, true)
                               })
        });

    parser.setArguments(argc, argv);
    std::string error;
    if (parser.parse(&error))
    {
        std::cout << parser.help() << std::endl << std::endl;

        std::cout << "Параметры командной строки разобраны успешно" << std::endl;

        std::cout << "Глобальные опции" << std::endl;

        for (const auto &option : parser.parsedGlobalOptions())
        {
            std::cout << "\t" << option.description();
            if (option.expectValue())
                std::cout << ": " << option.value();
            std::cout << std::endl;
        }

        std::cout << "Команда " << parser.parsedCommand().description()
                  << " - " << parser.parsedCommand().command() << std::endl;

        for (auto &option : parser.parsedCommand().options())
        {
            std::cout << "\t" << option.description();
            if (option.expectValue())
                std::cout << ": " << option.value();
            std::cout << std::endl;
        }

    }
    else
    {
        std::cout << "Ошибка разбора командной строки " << error << std::endl;
    }
}



