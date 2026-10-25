#include "ThreadMainSslServer.h"
#include <DaemonApplication.h>

int main(int argc, char *argv[])
{
    DaemonApplication<ThreadMainSslServer> application(argc, argv, "Сервер SSL");
    return application.run();
}
