#pragma once

#include "Logger.h"

using namespace Threader::Utils;

constexpr char MESSAGE_NAME_SEND_PING[] = "Message.Send.Ping";
constexpr char MESSAGE_NAME_PING[] = "Message.Ping";
constexpr char MESSAGE_NAME_PONG[] = "Message.Pong";

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Main"

MESSAGE_TEMPLATE(10, 0, MessageType::Message,
                 "Starting threads...")
MESSAGE_TEMPLATE(11, 0, MessageType::Message,
                 "All threads started")
MESSAGE_TEMPLATE(12, 0, MessageType::Message,
                 "Stopping threads...")
MESSAGE_TEMPLATE(13, 0, MessageType::Message,
                 "All threads stopped")

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Ping"

MESSAGE_TEMPLATE(20, 0, MessageType::Message,
                 "Ping thread started")
MESSAGE_TEMPLATE(21, 0, MessageType::Message,
                 "Ping thread finished")
MESSAGE_TEMPLATE(22, 0, MessageType::Message,
                 "Message %s received")
MESSAGE_TEMPLATE(23, 0, MessageType::Message,
                 "Message %s sent")

#undef LOG_SUBSYSTEM
#define LOG_SUBSYSTEM "Pong"

MESSAGE_TEMPLATE(30, 0, MessageType::Message,
                 "Pong thread started")
MESSAGE_TEMPLATE(31, 0, MessageType::Message,
                 "Pong thread finished")
MESSAGE_TEMPLATE(32, 0, MessageType::Message,
                 "Message %s received")
MESSAGE_TEMPLATE(33, 0, MessageType::Message,
                 "Message %s sent")
