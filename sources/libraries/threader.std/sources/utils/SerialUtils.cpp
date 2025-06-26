#include "SerialUtils.h"

#ifdef linux
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <unistd.h>
#endif

#ifdef WIN32
    #include <windows.h>
#endif

#include <string.h>

namespace Threader::Utils {

#ifdef WIN32

const std::string SERIAL_PORT_PREFFIX = "\\\\.\\";

const DWORD SERIAL_EVENTS_MASK_READ =
    EV_RXCHAR | EV_RXFLAG | EV_CTS | EV_DSR | EV_RLSD | EV_BREAK | EV_ERR | EV_RING | EV_RX80FULL;

const DWORD SERIAL_EVENTS_MASK_READ_WRITE = EV_RXCHAR | EV_RXFLAG | EV_TXEMPTY | EV_CTS | EV_DSR |
                                            EV_RLSD | EV_BREAK | EV_ERR | EV_RING | EV_RX80FULL;

#endif

Descriptor SerialUtils::openSerial(const std::string &deviceName,
                                   const uint32_t &baudRate,
                                   const bool &enableParity,
                                   const bool &twoStopBits,
                                   const uint32_t &dataSize)
{
    Descriptor serialPortDescriptor = INVALID_DESCRIPTOR;

#ifdef linux

    serialPortDescriptor = ::open(deviceName.c_str(), O_RDWR | O_NOCTTY, S_IRWXU);

    bool isOpened = serialPortDescriptor >= 0;

    if (isOpened) {
        struct termios options;
        memset(&options, 0, sizeof(options));

        tcgetattr(serialPortDescriptor, &options);

        cfmakeraw(&options);

        cfsetspeed(&options, baudRate);

        if (enableParity)
            options.c_cflag |= PARENB;
        else
            options.c_cflag &= ~uint(PARENB);

        if (twoStopBits)
            options.c_cflag |= CSTOPB;
        else
            options.c_cflag &= ~uint(CSTOPB);

        options.c_cflag &= ~uint(CSIZE);
        options.c_cflag |= dataSize;

        // отключение эха
        options.c_lflag = 0;

        // включение неблокирующего режима
        options.c_cc[VMIN] = 0;
        options.c_cc[VTIME] = 0;

        tcflush(serialPortDescriptor, TCIOFLUSH);
        tcsetattr(serialPortDescriptor, TCSANOW, &options);
    }

#endif

#ifdef WIN32
    // дополнение имени порта
    auto localDeviceName = deviceName;
    if (!deviceName.compare(0, SERIAL_PORT_PREFFIX.length(), SERIAL_PORT_PREFFIX))
        localDeviceName = SERIAL_PORT_PREFFIX + deviceName;

    // открытие порта
    serialPortDescriptor =
        CreateFileA((LPCSTR)localDeviceName.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
                    nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);

    if (serialPortDescriptor == INVALID_DESCRIPTOR)
        return serialPortDescriptor;

    PurgeComm(serialPortDescriptor, PURGE_RXABORT);
    PurgeComm(serialPortDescriptor, PURGE_RXCLEAR);
    DWORD errors = 0;
    ClearCommError(serialPortDescriptor, &errors, nullptr);

    // установка размера буферов приема/передачи
    if (!SetupComm(serialPortDescriptor, 1024, 1024)) {
        SerialUtils::closeSerial(serialPortDescriptor);
        return INVALID_DESCRIPTOR;
    }

    // получение настроек порта
    DCB dcb;
    if (!GetCommState(serialPortDescriptor, &dcb)) {
        SerialUtils::closeSerial(serialPortDescriptor);
        return INVALID_DESCRIPTOR;
    }

    // установка скорости порта
    dcb.BaudRate = baudRate;
    dcb.ByteSize = dataSize;
    dcb.Parity = (enableParity) ? ODDPARITY : NOPARITY;
    dcb.StopBits = (twoStopBits) ? TWOSTOPBITS : ONESTOPBIT;

    // установка настроек порта
    if (!SetCommState(serialPortDescriptor, &dcb)) {
        SerialUtils::closeSerial(serialPortDescriptor);
        return INVALID_DESCRIPTOR;
    }

    // получение значений таймаутов
    COMMTIMEOUTS timeouts;
    if (!GetCommTimeouts(serialPortDescriptor, &timeouts)) {
        SerialUtils::closeSerial(serialPortDescriptor);
        return INVALID_DESCRIPTOR;
    }

    // настройка параметров таймаутов таким образом,
    // чтобы ReadFile и WriteFile возвращали значения немедленно (?)
    timeouts.ReadIntervalTimeout = MAXWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

    // установка значений таймаутов
    if (!SetCommTimeouts(serialPortDescriptor, &timeouts)) {
        SerialUtils::closeSerial(serialPortDescriptor);
        return INVALID_DESCRIPTOR;
    }
#endif

    return serialPortDescriptor;
}

void SerialUtils::closeSerial(Descriptor descriptor)
{
#ifdef linux
    ::close(descriptor);
#endif
#ifdef _WIN32
    CloseHandle(descriptor);
#endif
}

int SerialUtils::inputBytesAvailable(Descriptor descriptor)
{
    int bytesAvailable = 0;
    if (INVALID_DESCRIPTOR == descriptor)
        return bytesAvailable;

#ifdef linux
    int errorCode = ioctl(descriptor, FIONREAD, &bytesAvailable);
    if (0 != errorCode)
        return 0;
#endif

#ifdef WIN32
    COMSTAT comStat;
    DWORD errors;
    if (ClearCommError(descriptor, &errors, &comStat))
        bytesAvailable = comStat.cbInQue;
#endif
    return bytesAvailable;
}

int SerialUtils::outputBytesAvailable(Descriptor descriptor)
{
    int bytesAvailable = 0;
    if (INVALID_DESCRIPTOR == descriptor)
        return bytesAvailable;

#ifdef linux
    int errorCode = ioctl(descriptor, TIOCOUTQ, &bytesAvailable);
    if (0 != errorCode)
        return 0;
#endif

#ifdef _WIN32
    COMSTAT comStat;
    DWORD errors;
    if (ClearCommError(descriptor, &errors, &comStat))
        bytesAvailable = comStat.cbOutQue;
#endif
    return bytesAvailable;
}

#ifdef WIN32

bool SerialUtils::setWaitMask(Descriptor descriptor, bool needsToWrite)
{
    if (needsToWrite)
        return SetCommMask(descriptor, SERIAL_EVENTS_MASK_READ_WRITE);
    else
        return SetCommMask(descriptor, SERIAL_EVENTS_MASK_READ);
}

#endif

}
