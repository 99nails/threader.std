
#include "SettingsBase.h"

namespace Threader::Utils
{


bool SettingsBundle::readBundle()
{
    return true;
}

bool SettingsBundle::writeBundle()
{
    return true;
}

std::string SettingsBundle::fileName() const
{
    return _fileName;
}

void SettingsBundle::setFileName(const std::string &fileName)
{
    _fileName = fileName;
}

bool SettingsBundle::read(IniFile &settings)
{
    bool result = true;
    for (const auto &s: _settingsBundle)
    {
        bool r = s->read(settings);
        if (!r)
            result = false;
    }
    return result;
}

bool SettingsBundle::write(IniFile &settings)
{
    bool result = true;
    for (const auto &s: _settingsBundle)
    {
        if (!s->write(settings))
            result = false;
    }
    return result;
}

SettingsBase::Ptr SettingsBundle::registerSettings(const SettingsBase::Ptr &settings)
{
    _settingsBundle.push_back(settings);
    return settings;
}

}
