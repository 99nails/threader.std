#pragma once

#include "SettingsGpsEmulator.h"

#include <SettingsBase.h>

using namespace Threader::Utils;


class SettingsExample5 : public SettingsBundleTemplate<SettingsGpsEmulator>
{
public:
    using Ptr = std::shared_ptr<SettingsExample5>;

public:
    explicit SettingsExample5(const std::string &fileName = std::string());

    static SettingsExample5 *instance();

private:
    static SettingsExample5 *_instance;
};
