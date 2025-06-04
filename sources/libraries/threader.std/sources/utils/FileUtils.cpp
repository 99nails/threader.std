#include "FileUtils.h"

#ifdef linux
#include <fcntl.h>
#endif

namespace Threader {

namespace Utils {

Descriptor FileUtils::fileOpenOrCreate(std::string fileName)
{
#ifdef linux
    Descriptor descriptor = ::open(fileName.c_str(), O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    return descriptor;
#endif
}

}
}
