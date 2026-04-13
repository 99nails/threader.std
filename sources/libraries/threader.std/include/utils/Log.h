#pragma once

#include "Logger.h"
#include "MessageConsumer.h"

namespace Threader::Utils {

using namespace Threader::Threads;

bool startLogSubsystem(const std::string &pathToBin,
                       MessageConsumer *consumer = nullptr,
                       bool console = false,
                       int level = 9);

void stopLogSubsystem();


void writeLog(const MessageLogTemplate messageTemplate, ...);

}
