#include "DaemonApplication.h"
#include "ThreadMainGps.h"

int main(int argc, char *argv[])
{
    return AppMain<ThreadMainGps>(argc, argv, "Эмуляция GPS");
}
