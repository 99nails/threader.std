#include "ThreadMainTimeServer.h"
#include <DaemonApplication.h>

int main(int argc, char *argv[])
{
    DaemonApplication<ThreadMainTimeServer> application(argc,
                                                        argv,
                                                        "Сервер времени GPS",
                                                        THREADER_VERSION4_STRING);

    COMM_OPTION("Имя устройства последовательного порта для получения данных GPS",
                "--serial", "-s", true, false);

    return application.run();
}
