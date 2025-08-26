#include "SettingsTimeServer.h"

constexpr char SECTION_NAME[] = "Server";

constexpr char PARAMETER_NAME_ALIVE_TIMEOUT[] = "AliveTimeout";
constexpr char PARAMETER_NAME_PORT[] = "Port";

SettingsTimeServer::SettingsTimeServer(uint32_t aliveTimeout, uint16_t port)
    : SettingsBase()
    , _aliveTimeout(aliveTimeout)
    , _port(port)
{
}

bool SettingsTimeServer::readFromIniFile(IniFile &iniFile)
{
    _aliveTimeout = iniFile.valueOf(SECTION_NAME, PARAMETER_NAME_ALIVE_TIMEOUT, _aliveTimeout);
    _port = iniFile.valueOf(SECTION_NAME, PARAMETER_NAME_PORT, _port);
    return true;
}

bool SettingsTimeServer::writeToIniFile(IniFile &settings)
{
    return true;
}

uint32_t SettingsTimeServer::aliveTimeout() const
{
    return _aliveTimeout;
}

void SettingsTimeServer::setAliveTimeout(uint32_t aliveTimeout)
{
    _aliveTimeout = aliveTimeout;
}

uint16_t SettingsTimeServer::port() const
{
    return _port;
}

void SettingsTimeServer::setPort(uint16_t port)
{
    _port = port;
}

