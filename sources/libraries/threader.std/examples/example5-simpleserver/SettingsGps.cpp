#include "SettingsGps.h"

constexpr char SECTION_NAME[] = "GPS";

constexpr char PARAMETER_NAME_PORT[] = "Port";

SettingsGps::SettingsGps(std::string port)
    : SettingsBase()
    , _port(port)
{
}

bool SettingsGps::readFromIniFile(IniFile &iniFile)
{
    _port = iniFile.value(SECTION_NAME, PARAMETER_NAME_PORT, _port);

    return true;
}

bool SettingsGps::writeToIniFile(IniFile &settings)
{
    return true;
}

std::string SettingsGps::port() const
{
    return _port;
}


