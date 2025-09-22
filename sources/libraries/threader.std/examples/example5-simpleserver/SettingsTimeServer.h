#pragma once

#include <SettingsBase.h>

using namespace Threader::Utils;

constexpr uint32_t DEFAULT_ALIVE_TIMEOUT = 30;
constexpr char DEFAULT_HOST[] = "localhost";
constexpr uint16_t DEFAULT_PORT = 56438;
constexpr uint32_t DEFAULT_RECONNECT_TIMEOUT = 10;

class SettingsTimeServer : public SettingsBase
{
public:
    using Ptr = std::shared_ptr<SettingsTimeServer>;

public:
    explicit SettingsTimeServer(const uint32_t &aliveTimeout = DEFAULT_ALIVE_TIMEOUT,
                                const std::string &host = DEFAULT_HOST,
                                const uint16_t port = DEFAULT_PORT,
                                const uint32_t &reconnectTimeout = DEFAULT_RECONNECT_TIMEOUT);

    bool readFromIniFile(IniFile &iniFile) override;
    bool writeToIniFile(IniFile &settings) override;

    uint32_t aliveTimeout() const;
    void setAliveTimeout(uint32_t aliveTimeout);

    const std::string &host() const;
    void setHost(const std::string &newHost);

    uint16_t port() const;
    void setPort(uint16_t port);

    uint32_t reconnectTimeout() const;
    void setReconnectTimeout(uint32_t newReconnectTimeout);

private:
    uint32_t _aliveTimeout;
    std::string _host;
    uint16_t _port;
    uint32_t _reconnectTimeout;
};
