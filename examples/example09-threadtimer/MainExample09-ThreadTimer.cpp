#include "ThreadMainTimer.h"
#include <DaemonApplication.h>

int main(int argc, char *argv[])
{
    DaemonApplication<ThreadMainTimer> application(argc, argv, "Поточные таймеры");
    return application.run();
}
