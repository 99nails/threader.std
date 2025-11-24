#include "DaemonApplication.h"
#include "ThreadMainGps.h"

int main(int argc, char *argv[])
{
    return AppMain<ThreadMainGps>(argc, argv, "gps-emulator", "Эмуляция GPS");
}
