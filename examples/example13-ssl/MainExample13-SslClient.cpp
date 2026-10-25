#include "ThreadMainSslClient.h"
#include <DaemonApplication.h>

int main(int argc, char *argv[])
{
    DaemonApplication<ThreadMainSslClient> application(argc, argv, "Клиент SSL");
    return application.run();
}
