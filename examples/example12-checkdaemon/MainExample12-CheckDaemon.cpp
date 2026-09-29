#include "DBusUtils.h"

#include <iostream>

using namespace Threader::Utils;

int main(int argc, char** argv)
{
    std::string unit = (argc > 1) ? argv[1] : "NetworkManager.service";

    SystemDServiceState state{};

    if (!SystemDUtils::getServiceState(unit, state))
    {
        std::cerr << "Служба " << unit << " не найдена\n";
        return 1;
    }

    std::cout << "Unit:         " << state.unit << "\n";
    std::cout << "Path:         " << state.path << "\n\n";

    std::cout << "Description:  " << state.description << "\n";
    std::cout << "ActiveState:  " << state.activeState << "\n";
    std::cout << "SubState:     " << state.subState    << "\n";
    std::cout << "MainPID:      " << state.mainPID     << "\n";
    std::cout << "StatusText:   " << state.statusText  << "\n";

    return 0;
}
