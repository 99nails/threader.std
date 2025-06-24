#include "ThreadUtils.h"

#ifdef linux
#include <fcntl.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

namespace Threader {

namespace Utils {

THREAD_ID ThreadUtils::currentThreadId()
{
#ifdef _WIN32
    return GetCurrentThreadId();
#else
    return syscall(SYS_gettid);
#endif
}

}}
