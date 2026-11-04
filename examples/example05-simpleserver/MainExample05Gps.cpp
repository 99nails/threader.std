#include "DaemonApplication.h"
#include "ThreadMainGps.h"

int main(int argc, char *argv[])
{
    DaemonApplication<ThreadMainGps> application(argc,
                                                 argv,
                                                 "Эмуляция GPS",
                                                 THREADER_VERSION4_STRING);

    COMM_OPTION("Имя устройства последовательного порта для отправки данных GPS",
                "--serial", "-s", true, false);

    return application.run();
}
