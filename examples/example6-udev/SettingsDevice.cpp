#include "SettingsDevice.h"

constexpr char SECTION_NAME[] = "Device";

constexpr char PARAMETER_NAME_PID[] = "PID";
constexpr char PARAMETER_NAME_VID[] = "VID";

SettingsDevice::SettingsDevice(const std::string &pid,
                               const std::string &vid)
    : SettingsBase()
    , _pid(pid)
    , _vid(vid)
{
}

bool SettingsDevice::readFromIniFile(IniFile &iniFile)
{
    _pid = iniFile.value(SECTION_NAME, PARAMETER_NAME_PID, _pid);
    _vid = iniFile.value(SECTION_NAME, PARAMETER_NAME_VID, _vid);

    return true;
}

bool SettingsDevice::writeToIniFile(IniFile &settings)
{
    return true;
}

const std::string &SettingsDevice::pid() const
{
    return _pid;
}

const std::string &SettingsDevice::vid() const
{
    return _vid;
}


