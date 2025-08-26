#pragma once

#include <SettingsBase.h>

using namespace Threader::Utils;

constexpr char DEFAULT_GPS_PORT[] = "/dev/pts/2";

class SettingsGps : public SettingsBase
{
public:
    using Ptr = std::shared_ptr<SettingsGps>;

public:
    explicit SettingsGps(std::string port = DEFAULT_GPS_PORT);

    bool readFromIniFile(IniFile &iniFile) override;
    bool writeToIniFile(IniFile &settings) override;

    std::string port() const;
private:
    std::string _port;
};
