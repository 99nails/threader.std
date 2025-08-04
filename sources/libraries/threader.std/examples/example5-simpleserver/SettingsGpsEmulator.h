#pragma once

#include <SettingsBase.h>

using namespace Threader::Utils;

class SettingsGpsEmulator : public SettingsBase
{
public:
    using Ptr = std::shared_ptr<SettingsGpsEmulator>;

public:
    explicit SettingsGpsEmulator();

    bool readFromIniFile(IniFile &iniFile) override;
    bool writeToIniFile(IniFile &settings) override;

    std::string portName() const;

    uint64_t period() const;

    double latitude() const;

    double longitude() const;

private:
    std::string _portName;
    uint64_t _period;
    double _latitude;
    double _longitude;
};
