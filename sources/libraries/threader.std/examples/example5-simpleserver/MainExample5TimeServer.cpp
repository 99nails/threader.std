#include "ThreadMainTimeServer.h"
#include <DaemonApplication.h>

int main(int argc, char *argv[])
{
    AppMain<ThreadMainTimeServer>(argc, argv);
}
