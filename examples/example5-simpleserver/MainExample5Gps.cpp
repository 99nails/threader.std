#include "DaemonApplication.h"
#include "ThreadMainGps.h"

int main(int argc, char *argv[])
{
    DaemonApplication<ThreadMainGps> application(argc, argv, "Эмуляция GPS");
    return application.run();
}
