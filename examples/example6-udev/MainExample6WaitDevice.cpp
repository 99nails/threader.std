#include "ThreadMainWaitDevice.h"

#include "DaemonApplication.h"

int main(int argc, char *argv[])
{
    DaemonApplication<ThreadMainWaitDevice> application(argc, argv, "Ожидание устройства");
    return application.run();
}
