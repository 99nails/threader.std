#include "ThreadMainTimeClient.h"
#include <DaemonApplication.h>

int main(int argc, char *argv[])
{
    DaemonApplication<ThreadMainTimeClient> application(argc, argv, "Клиент сервера времени GPS");
    return application.run();
}
