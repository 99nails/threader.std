#include "DaemonApplicationInfo.h"

namespace Threader::Utils
{

DaemonApplicationInfo::DaemonApplicationInfo()
{

}

void DaemonApplicationInfo::setArguments(int argc, char *argv[])
{
    _arguments.reserve(argc);
    for (int i = 0; i < argc; i++)
    {
        _arguments[i] = argv[i];
    }
}

}
