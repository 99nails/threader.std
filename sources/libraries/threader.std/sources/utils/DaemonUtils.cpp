#include "DaemonUtils.h"

#include "FileUtils.h"
#include "StdStringUtils.h"

#include <errno.h>
#include <iostream>
#include <string.h>

constexpr char SERVICE_TEMPLATE[] = R"STRING(
[Unit]
Description=%s
After=network.target

[Service]
Type=simple
ExecStart=%s
ExecReload=/bin/kill -HUP $MAINPID
User=root
StandardOutput=syslog
StandardError=syslog
SyslogIdentifier=%s

[Install]
WantedBy=multi-user.target
)STRING";

constexpr char SERVICE_FILENAME_TEMPLATE[] = "/etc/systemd/system/%s.service";

bool Threader::Utils::DaemonUtils::registerDaemon(const std::string &serviceName,
                                                  const std::string &binaryFileName,
                                                  const std::string &description)
{
    auto serviceFileName = format(SERVICE_FILENAME_TEMPLATE, serviceName.c_str());
    auto descriptor = FileUtils::fileCreate(serviceFileName);
    if (INVALID_DESCRIPTOR != descriptor)
    {
        auto content = format(SERVICE_TEMPLATE, description.c_str(), binaryFileName.c_str(), serviceName.c_str());
        FileUtils::fileWrite(descriptor, content);
        FileUtils::fileClose(descriptor);

        std::cout << format("Демон %s успешно зарегистрирован", serviceName.c_str()) << std::endl;

        return true;
    }

    std::cout << "Ошибка регистрации демона: " << std::string(strerror(errno)) << std::endl;
    return false;
}

bool Threader::Utils::DaemonUtils::unregisterDaemon(const std::string &serviceName)
{
    auto serviceFileName = format(SERVICE_FILENAME_TEMPLATE, serviceName.c_str());
    if (!FileUtils::isFileExists(serviceFileName))
    {
        std::cout << format("Ошибка снятия регистрации демона: файл %s не найден", serviceFileName.c_str()) << std::endl;
        return false;
    }

    if (!FileUtils::fileDelete(serviceFileName))
    {
        std::cout << "Ошибка снятия регистрации демона: " << std::string(strerror(errno)) << std::endl;
        return false;
    }

    std::cout << format("Демон %s успешно снят с регистрации", serviceName.c_str()) << std::endl;
    return true;
}
