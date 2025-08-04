#include "SettingsExample5.h"

#include "FileUtils.h"

SettingsExample5 *SettingsExample5::_instance = nullptr;

SettingsExample5::SettingsExample5(const std::string &fileName)
    : SettingsBundleTemplate<SettingsGpsEmulator>()
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
}

SettingsExample5 *SettingsExample5::instance()
{
    return _instance;
}
