#pragma once

#include <SettingsBase.h>

using namespace Threader::Utils;

constexpr uint32_t DEFAULT_ALIVE_TIMEOUT = 30;
constexpr uint16_t DEFAULT_PORT = 56438;

class SettingsTimeServer : public SettingsBase
{
public:
    using Ptr = std::shared_ptr<SettingsTimeServer>;

public:
    explicit SettingsTimeServer(uint32_t aliveTimeout = DEFAULT_ALIVE_TIMEOUT,
                                uint16_t port = DEFAULT_PORT);

    bool readFromIniFile(IniFile &iniFile) override;
    bool writeToIniFile(IniFile &settings) override;

    uint32_t aliveTimeout() const;
    void setAliveTimeout(uint32_t aliveTimeout);

    uint16_t port() const;
    void setPort(uint16_t port);

private:
    uint32_t _aliveTimeout;
    uint16_t _port;
};
