#include "ThreadMainWaitDevice.h"

#include "DaemonApplication.h"

int main(int argc, char *argv[])
{
    return AppMain<ThreadMainWaitDevice>(argc, argv, "wait-device", "Ожидание устройства");
}
