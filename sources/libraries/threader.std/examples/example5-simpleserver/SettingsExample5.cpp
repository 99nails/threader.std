#include "SettingsExample5.h"

#include "FileUtils.h"

SettingsExample5 *SettingsExample5::_instance = nullptr;

SettingsExample5::SettingsExample5(const std::string &fileName)
    : SettingsBundle()
    , _settingsGpsEmulator(std::make_shared<SettingsGpsEmulator>())
{
    _instance = this;

    std::string localFileName = fileName;
    if (localFileName.empty()) {

        auto path = FileUtils::absolutePath(SETTINGS_FOLDER);

        if (!FileUtils::isPathExists(path))
            FileUtils::makeDir(path);

        localFileName = path + FileUtils::DELIMITER + SETTINGS_FILENAME;
    }
    else
    {
        localFileName = fileName;
    }

    setFileName(localFileName);

    registerSettings(_settingsGpsEmulator);
}

bool SettingsExample5::readBundle()
{
    if (FileUtils::isFileExists(fileName()))
    {
        IniFile iniFile;
        iniFile.fileOpen(fileName());
        return SettingsBundle::read(iniFile);
    }
    return false;
}

bool SettingsExample5::writeBundle()
{
    return true;
}

SettingsExample5 *SettingsExample5::instance()
{
    return _instance;
}

SettingsGpsEmulator *SettingsExample5::settingsGpsEmulator() const
{
    return _settingsGpsEmulator.get();
}
