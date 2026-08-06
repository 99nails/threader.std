#include "SettingsGpsEmulator.h"

constexpr char SECTION_NAME[] = "GPS.Emulator";

constexpr char PARAMETER_NAME_PORT[] = "Port";
constexpr char PARAMETER_NAME_PERIOD[] = "Period";
constexpr char PARAMETER_NAME_LATITUDE[] = "Latitude";
constexpr char PARAMETER_NAME_LONGITUDE[] = "Longitude";

SettingsGpsEmulator::SettingsGpsEmulator(std::string portName, uint64_t period, double latitude, double longitude)
    : SettingsBase()
    , _latitude(latitude)
    , _longitude(longitude)
    , _period(period)
    , _port(portName)
{
}

bool SettingsGpsEmulator::readFromIniFile(IniFile &iniFile)
{
    _latitude = iniFile.value(SECTION_NAME, "Latitude", _latitude);
    _longitude = iniFile.value(SECTION_NAME, "Longitude", _longitude);
    _period = iniFile.value(SECTION_NAME, "Period", _period) * 1000;
    _port = iniFile.value(SECTION_NAME, "Port", "");

    return true;
}

bool SettingsGpsEmulator::writeToIniFile(IniFile &settings)
{
    return true;
}

double SettingsGpsEmulator::latitude() const
{
    return _latitude;
}

double SettingsGpsEmulator::longitude() const
{
    return _longitude;
}

uint64_t SettingsGpsEmulator::period() const
{
    return _period;
}

std::string SettingsGpsEmulator::port() const
{
    return _port;
}


