#pragma once

#include <SettingsBase.h>

using namespace Threader::Utils;

constexpr char DEFAULT_PORT[] = "/dev/pts/2";
constexpr uint64_t DEFAULT_PERIOD = 2;
constexpr double DEFAULT_LATITUDE = 59.9386;
constexpr double DEFAULT_LONGITUDE = 30.3141;

class SettingsGpsEmulator : public SettingsBase
{
public:
    using Ptr = std::shared_ptr<SettingsGpsEmulator>;

public:
    explicit SettingsGpsEmulator(std::string portName = DEFAULT_PORT,
                                 uint64_t period = DEFAULT_PERIOD,
                                 double latitude = DEFAULT_LATITUDE,
                                 double longitude = DEFAULT_LONGITUDE);

    bool readFromIniFile(IniFile &iniFile) override;
    bool writeToIniFile(IniFile &settings) override;

    double latitude() const;

    double longitude() const;

    uint64_t period() const;

    std::string port() const;
private:
    double _latitude;
    double _longitude;
    uint64_t _period;
    std::string _port;
};
