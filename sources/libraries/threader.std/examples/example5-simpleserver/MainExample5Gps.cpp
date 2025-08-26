#include "DaemonApplication.h"
#include "ThreadMainGps.h"

int main(int argc, char *argv[])
{
    AppMain<ThreadMainGps>(argc, argv);
}
