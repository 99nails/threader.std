#include "ThreadMainSslServer.h"
#include <DaemonApplication.h>

int main(int argc, char *argv[])
{
    DaemonApplication<ThreadMainSslServer> application(argc,
                                                       argv,
                                                       "Сервер SSL",
                                                       THREADER_VERSION4_STRING);
    return application.run();
}
