#include "UdevEnumerator.h"

using namespace Threader::Utils;

int main(int argc, char *argv[])
{
    auto enumerator = UdevEnumerator();
    enumerator.enumerate();

    return 0;
}
