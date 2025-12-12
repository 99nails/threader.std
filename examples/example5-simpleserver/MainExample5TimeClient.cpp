#include "ThreadMainTimeClient.h"
#include <DaemonApplication.h>

int main(int argc, char *argv[])
{
    return AppMain<ThreadMainTimeClient>(argc, argv, "Клиент сервера времени GPS");
}
