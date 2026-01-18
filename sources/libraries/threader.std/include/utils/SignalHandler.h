#pragma once

#include <string>

#ifdef linux
void setErrorSignalHandler(const char* arg0);
#endif

std::string demangle(const char* name);

void setThreadInfo(const std::string &info);
