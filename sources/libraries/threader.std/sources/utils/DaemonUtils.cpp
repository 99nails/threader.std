#include "DaemonUtils.h"

#include "FileUtils.h"
#include "StdStringUtils.h"

#include <cerrno>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>

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

namespace Threader::Utils {

constexpr char SERVICE_FILENAME_TEMPLATE[] = "/etc/systemd/system/%s.service";

sem_t *DaemonUtils::_semaphore = SEM_FAILED;

bool DaemonUtils::daemonRegister(const std::string &serviceName,
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

    std::cout << "Ошибка регистрации демона: " << std::string(std::strerror(errno)) << std::endl;
    return false;
}

bool DaemonUtils::daemonUnregister(const std::string &serviceName)
{
    auto serviceFileName = format(SERVICE_FILENAME_TEMPLATE, serviceName.c_str());
    if (!FileUtils::isFileExists(serviceFileName))
    {
        std::cout << format("Ошибка снятия регистрации демона: файл %s не найден", serviceFileName.c_str()) << std::endl;
        return false;
    }

    if (!FileUtils::fileDelete(serviceFileName))
    {
        std::cout << "Ошибка снятия регистрации демона: " << std::string(std::strerror(errno)) << std::endl;
        return false;
    }

    std::cout << format("Демон %s успешно снят с регистрации", serviceName.c_str()) << std::endl;
    return true;
}

bool DaemonUtils::daemonSetRunning(const std::string &serviceName,
                                                    const bool isRunning)
{
    auto semaphoreName = format("semaphore.daemon.%s", serviceName.c_str());
    if (isRunning)
    {
        _semaphore = sem_open(semaphoreName.c_str(), O_CREAT | O_EXCL, S_IRWXU, 1);
        return (_semaphore != SEM_FAILED);
    }

    if (_semaphore != SEM_FAILED)
    {
        sem_unlink(semaphoreName.c_str());
        _semaphore = SEM_FAILED;
    }
    return true;
}

bool DaemonUtils::daemonIsRunning(const std::string &serviceName)
{
    auto semaphoreName = format("semaphore.daemon.%s", serviceName.c_str());
    _semaphore = sem_open(semaphoreName.c_str(), O_CREAT | O_EXCL, S_IRWXU, 1);
    if (_semaphore == SEM_FAILED)
    {
        if (EEXIST == errno)
            return true;
    }

    sem_unlink(semaphoreName.c_str());

    return false;
}

}
