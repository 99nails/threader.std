#pragma once

#include <SettingsBase.h>

using namespace Threader::Utils;

constexpr char DEFAULT_PID[] = "ea60";
constexpr char DEFAULT_VID[] = "10c4";

class SettingsDevice : public SettingsBase
{
public:
    using Ptr = std::shared_ptr<SettingsDevice>;

public:
    explicit SettingsDevice(const std::string &pid = DEFAULT_PID,
                            const std::string &vid = DEFAULT_VID);

    bool readFromIniFile(IniFile &iniFile) override;
    bool writeToIniFile(IniFile &settings) override;

    const std::string &pid() const;

    const std::string &vid() const;

private:
    std::string _pid;
    std::string _vid;
};
