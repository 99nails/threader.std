#pragma once

#ifdef linux

using Descriptor = int;

constexpr Descriptor INVALID_DESCRIPTOR = -1;

#endif

#ifdef WIN32

// Настройки заголовочника <winsock.h>, который подключен в <windows.h>, приводят к
// предупреждению при компиляции в <winsock2.h>: Please include winsock2.h before windows.h
// Удаление макроса _WINSOCKAPI_ дезавуирует данную проблему
#undef _WINSOCKAPI_

#include <winsock2.h>
#include <windows.h>
#include <sdkddkver.h>

using Descriptor = HANDLE;

const Descriptor INVALID_DESCRIPTOR = INVALID_HANDLE_VALUE;

#endif

using THREAD_ID = long;

enum class TcpSocketType
{
    Server,
    Client
};

enum class UdpSocketType
{
    Receiver,
    Sender
};
