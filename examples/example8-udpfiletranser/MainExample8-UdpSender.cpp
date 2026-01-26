#include "ThreadMainUdpSender.h"
#include <DaemonApplication.h>

int main(int argc, char *argv[])
{
    DaemonApplication<ThreadMainUdpSender> application(argc, argv, "Отправка файлов по UDP");
    return application.run();
}
