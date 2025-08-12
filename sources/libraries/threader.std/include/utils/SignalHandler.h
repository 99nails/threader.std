#pragma once

#include <string>

#ifdef linux
void setSignalHandler(const char* arg0);
#endif

std::string demangle(const char* name);


