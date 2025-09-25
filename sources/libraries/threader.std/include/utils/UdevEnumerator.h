#pragma once

#include <libudev.h>
#include <string>
#include <map>

namespace Threader::Utils
{

constexpr char SUBSYSTEM_GPIO[] = "gpio";
constexpr char SUBSYSTEM_WLAN[] = "net/wl";
constexpr char SUBSYSTEM_TTY[] = "tty";

class UdevEnumerator
{
public:
    explicit UdevEnumerator();
    virtual ~UdevEnumerator();

    void enumerate();

private:
    void initializeContext();
    void releaseContext();

private:
    struct udev *_context;
    std::string _subsystem;
    std::map<std::string, std::string> _subsystems;
};

}
