#include "ThreadMainTimeServer.h"
#include <DaemonApplication.h>

int main(int argc, char *argv[])
{
    return AppMain<ThreadMainTimeServer>(argc, argv, "Сервер времени GPS");
}
