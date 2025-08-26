#pragma once

#include "IniFile.h"
#include "FileUtils.h"

#include <map>
#include <vector>

#include <memory>

namespace Threader::Utils
{

enum class SettingsLocation
{
    HomeDir,
    NearExecutable,
    DirectReference
};

constexpr char SETTINGS_FOLDER[] = "../config";
constexpr char SETTINGS_FILENAME[] = "config.ini";

class SettingsBase
{
public:
    using Ptr = std::shared_ptr<SettingsBase>;

public:
    virtual ~SettingsBase() = default;
    virtual bool readFromIniFile(IniFile &settings) = 0;
    virtual bool writeToIniFile(IniFile &settings) = 0;
};


using SettingsBaseVector = std::vector<SettingsBase::Ptr>;


using SettingsBaseHash = std::map<std::string, SettingsBase::Ptr>;


class SettingsBundle : public SettingsBase
{
public:
    using Ptr = std::shared_ptr<SettingsBundle>;

public:
    virtual bool readBundle();
    virtual bool writeBundle();

    std::string fileName() const;
    void setFileName(const std::string &fileName);

    template <typename SettingsType>
    typename SettingsType::Ptr settings()
    {
        for (const auto &s : _settingsBundle)
        {
            auto result = std::dynamic_pointer_cast<SettingsType>(s);
            if (result)
                return result;
        }
        return nullptr;
    }

    bool readFromIniFile(IniFile &settings) override;

    bool writeToIniFile(IniFile &settings) override;

protected:
    SettingsBase::Ptr registerSettings(const SettingsBase::Ptr &settings);

private:
    SettingsBaseVector _settingsBundle;
    std::string _fileName = std::string();
};

template<class Class>
class SettingHolder
{
public:
    explicit SettingHolder()
        : _settings(std::make_shared<Class>())
    {}

    using Ptr = std::shared_ptr<Class>;

    Ptr settings() const
    {
        return _settings;
    }
private:
    Ptr _settings;
};


template<class... SettingsClasses>
class SettingsBundleTemplate : public SettingHolder<SettingsClasses>...
{
public:
    explicit SettingsBundleTemplate(const std::string &fileName = std::string())
        : SettingHolder<SettingsClasses>()...
    {
        setFileName(fileName);
    }

    template<class SettingClass>
    typename SettingHolder<SettingClass>::Ptr get()
    {
        return this->SettingHolder<SettingClass>::settings();
    }

    bool read()
    {
        if (FileUtils::isFileExists(_fileName))
        {
            IniFile iniFile;
            iniFile.fileOpen(_fileName);
            return readFromIniFile(iniFile);
        }
        return false;
    }

    std::string fileName() const
    {
        return _fileName;
    }

    void setFileName(const std::string &fileName)
    {
        std::string localFileName = fileName;
        if (localFileName.empty()) {

            auto path = FileUtils::absolutePath(SETTINGS_FOLDER);

            if (!FileUtils::isPathExists(path))
                FileUtils::makeDir(path);

            localFileName = path + FileUtils::DELIMITER + SETTINGS_FILENAME;
        }
        else
        {
            localFileName = FileUtils::absolutePath(fileName);
        }

        _fileName = localFileName;
    }


protected:
    bool readFromIniFile(IniFile &settings)
    {
        bool results[] =
        {
            get<SettingsClasses>()->readFromIniFile(settings)...
        };

        for (const auto &result : results)
        {
            if (not result)
                return false;
        }

        return true;
    }

private:
    std::string _fileName = std::string();
};

}
