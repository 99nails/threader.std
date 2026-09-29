#include "ThreadMainTimeClient.h"
#include <DaemonApplication.h>

int main(int argc, char *argv[])
{
    DaemonApplication<ThreadMainTimeClient> application(argc,
                                                        argv,
                                                        "Клиент сервера времени GPS",
                                                        THREADER_VERSION4_STRING);
    return application.run();
}
