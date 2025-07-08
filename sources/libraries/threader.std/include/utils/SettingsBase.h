#pragma once

#include "IniFile.h"

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

class SettingsBase
{
public:
    using Ptr = std::shared_ptr<SettingsBase>;

public:
    virtual ~SettingsBase() = default;
    virtual bool read(IniFile &settings) = 0;
    virtual bool write(IniFile &settings) = 0;
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

protected:

    bool read(IniFile &settings) override;

    bool write(IniFile &settings) override;

    SettingsBase::Ptr registerSettings(const SettingsBase::Ptr &settings);

private:
    SettingsBaseVector _settingsBundle;
    std::string _fileName = std::string();
};

}
