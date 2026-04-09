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
                                 const std::string &description,
                                 std::string *resultString)
{
    auto serviceFileName = format(SERVICE_FILENAME_TEMPLATE, serviceName.c_str());
    auto descriptor = FileUtils::fileCreate(serviceFileName);
    if (INVALID_DESCRIPTOR != descriptor)
    {
        auto content = format(SERVICE_TEMPLATE, description.c_str(), binaryFileName.c_str(), serviceName.c_str());
        FileUtils::fileWrite(descriptor, content);
        FileUtils::fileClose(descriptor);

        if (resultString)
            *resultString = format("Служба %s успешно зарегистрирована", serviceName.c_str());

        return true;
    }

    if (resultString)
        *resultString = std::string(std::strerror(errno));
    return false;
}

bool DaemonUtils::daemonUnregister(const std::string &serviceName,
                                   std::string *resultString)
{
    auto serviceFileName = format(SERVICE_FILENAME_TEMPLATE, serviceName.c_str());
    if (!FileUtils::isFileExists(serviceFileName))
    {
        if (resultString)
            *resultString = format("Ошибка снятия регистрации службы: файл %s не найден", serviceFileName.c_str());
        return false;
    }

    if (!FileUtils::fileDelete(serviceFileName))
    {
        if (resultString)
            *resultString = std::string(std::strerror(errno));
        return false;
    }

    if (resultString)
        *resultString = format("Служба %s успешно снята с регистрации", serviceName.c_str());
    return true;
}

bool DaemonUtils::daemonSetRunning(const std::string &serviceName,
                                   const bool isRunning)
{
    if (serviceName.empty())
        return false;

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
