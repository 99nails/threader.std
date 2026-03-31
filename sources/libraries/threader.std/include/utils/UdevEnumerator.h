#pragma once

#include "UdevDeviceInfo.h"

#include <functional>
#include <string>

#include <libudev.h>

namespace Threader::Utils
{

using UdevEnumerationCallback = std::function<bool(const UdevDeviceInfo &)>;

class UdevEnumerator
{
public:
    explicit UdevEnumerator(const std::vector<std::string> &matchedSubsystems = {},
                            const std::vector<std::string> &unmatchedSubsystems = {},
                            const PropertyVector &matchedProperties = {});
    virtual ~UdevEnumerator();

    std::vector<std::string> registeredSubsystems() const;

    std::vector<std::string> matchedSubsystems() const;
    void setMatchedSubsystems(const std::vector<std::string> &matchedSubsystems);

    std::vector<std::string> unmatchedSubsystems() const;
    void setUnmatchedSubsystems(const std::vector<std::string> &unmatchedSubsystems);

    void enumerate(UdevEnumerationCallback &handler);

    PropertyVector matchedProperties() const;
    void setMatchedProperties(const PropertyVector &matchedProperties);

private:
    void initializeContext();
    void releaseContext();

    void collectSubsystemNames();
private:
    struct udev *_context;
    std::vector<std::string> _registeredSubsystems;
    std::vector<std::string> _matchedSubsystems;
    std::vector<std::string> _unmatchedSubsystems;

    PropertyVector _matchedProperties;
};

}
