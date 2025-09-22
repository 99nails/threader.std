#include "SettingsTimeServer.h"

constexpr char SECTION_NAME[] = "Server";

constexpr char PARAMETER_NAME_ALIVE_TIMEOUT[] = "AliveTimeout";
constexpr char PARAMETER_NAME_PORT[] = "Port";
constexpr char PARAMETER_NAME_RECONNECT_TIMEOUT[] = "ReconnectTimeout";

SettingsTimeServer::SettingsTimeServer(const uint32_t &aliveTimeout,
                                       const std::string &host,
                                       const uint16_t port,
                                       const uint32_t &reconnectTimeout)
    : SettingsBase()
    , _aliveTimeout(aliveTimeout)
    , _host(host)
    , _port(port)
    , _reconnectTimeout(reconnectTimeout)
{
}

bool SettingsTimeServer::readFromIniFile(IniFile &iniFile)
{
    _aliveTimeout = iniFile.valueOf(SECTION_NAME, PARAMETER_NAME_ALIVE_TIMEOUT, _aliveTimeout);
    _port = iniFile.valueOf(SECTION_NAME, PARAMETER_NAME_PORT, _port);
    _reconnectTimeout = iniFile.valueOf(SECTION_NAME, PARAMETER_NAME_RECONNECT_TIMEOUT, _reconnectTimeout);
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

const std::string &SettingsTimeServer::host() const
{
    return _host;
}

void SettingsTimeServer::setHost(const std::string &newHost)
{
    _host = newHost;
}

uint16_t SettingsTimeServer::port() const
{
    return _port;
}

void SettingsTimeServer::setPort(uint16_t port)
{
    _port = port;
}

uint32_t SettingsTimeServer::reconnectTimeout() const
{
    return _reconnectTimeout;
}

void SettingsTimeServer::setReconnectTimeout(uint32_t newReconnectTimeout)
{
    _reconnectTimeout = newReconnectTimeout;
}
