#pragma once

#include "SettingsGpsEmulator.h"

#include <SettingsBase.h>

using namespace Threader::Utils;

constexpr char SETTINGS_FOLDER[] = "../config";
constexpr char SETTINGS_FILENAME[] = "example5.ini";


class SettingsExample5 : public SettingsBundle
{
public:
    using Ptr = std::shared_ptr<SettingsExample5>;

public:
    explicit SettingsExample5(const std::string &fileName = std::string());

    bool readBundle() override;
    bool writeBundle() override;

    static SettingsExample5 *instance();

    SettingsGpsEmulator *settingsGpsEmulator() const;

private:
    static SettingsExample5 *_instance;

    SettingsGpsEmulator::Ptr _settingsGpsEmulator;
};
