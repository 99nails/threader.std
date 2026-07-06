#pragma once

#include "Logger.h"
#include "MessageConsumer.h"

namespace Threader::Utils {

using namespace Threader::Threads;

bool startLogSubsystem(const std::string &pathToBin,
                       MessageConsumer *consumer = nullptr,
                       const bool console = false,
                       const int level = 9,
                       const bool clearHistory = true,
                       const int historyDepth = 30,
                       const bool pack = true,
                       const int packDepth = 2);

void stopLogSubsystem();


void writeLog(const MessageLogTemplate messageTemplate, ...);

}
