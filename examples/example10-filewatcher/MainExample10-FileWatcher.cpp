#include "ThreadMainFileWatcher.h"
#include <DaemonApplication.h>

int main(int argc, char *argv[])
{
    DaemonApplication<ThreadMainFileWatcher> application(argc, argv, "Наблюдение за процессами");
    return application.run();
}
