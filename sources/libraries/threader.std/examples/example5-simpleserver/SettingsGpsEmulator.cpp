#include "SettingsGpsEmulator.h"

constexpr char SECTION_NAME[] = "GPS.Emulator";

constexpr char PARAMETER_NAME_PORT[] = "Port";
constexpr char PARAMETER_NAME_PERIOD[] = "Period";
constexpr char PARAMETER_NAME_LATITUDE[] = "Latitude";
constexpr char PARAMETER_NAME_LONGITUDE[] = "Longitude";

SettingsGpsEmulator::SettingsGpsEmulator()
    : SettingsBase()
    , _portName(std::string())
    , _period(2)
    , _latitude(0.)
    , _longitude(0.)
{
}

bool SettingsGpsEmulator::read(IniFile &iniFile)
{
    _portName = iniFile.value(SECTION_NAME, "Port", "");
    _period = iniFile.valueOf(SECTION_NAME, "Period", _period) * 1000;
    _latitude = iniFile.valueOf(SECTION_NAME, "Latitude", _latitude);
    _longitude = iniFile.valueOf(SECTION_NAME, "Longitude", _longitude);

    return true;
}

bool SettingsGpsEmulator::write(IniFile &settings)
{
    return true;
}

std::string SettingsGpsEmulator::portName() const
{
    return _portName;
}

uint64_t SettingsGpsEmulator::period() const
{
    return _period;
}

double SettingsGpsEmulator::latitude() const
{
    return _latitude;
}

double SettingsGpsEmulator::longitude() const
{
    return _longitude;
}
