#include "ThreadMainTimeServer.h"
#include <DaemonApplication.h>

int main(int argc, char *argv[])
{
    DaemonApplication<ThreadMainTimeServer> application(argc,
                                                        argv,
                                                        "Сервер времени GPS",
                                                        THREADER_VERSION4_STRING);
    return application.run();
}
