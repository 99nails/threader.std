#include "FileUtils.h"

#ifdef linux
#include <fcntl.h>
#endif

#include <stdio.h>
#include <sys/stat.h>

namespace Threader {

namespace Utils {

Descriptor FileUtils::fileOpenOrCreate(const std::string &fileName)
{
#ifdef linux
    Descriptor descriptor = ::open(fileName.c_str(), O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    return descriptor;
#endif
}

bool FileUtils::isPathExists(const std::string &path)
{
    struct stat sb;
    return stat(path.c_str(), &sb) == 0
            && S_ISDIR(sb.st_mode);
}

}
}
