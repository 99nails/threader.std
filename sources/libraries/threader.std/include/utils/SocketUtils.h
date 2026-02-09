#pragma once

#include "ThreadsCommon.h"

#ifdef linux
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef WIN
#include "windows.h"
#endif

#include <list>
#include <set>
#include <string>

namespace Threader::Utils {

typedef struct InterfaceInfo
{
    InterfaceInfo(std::string interfaceName_ = std::string(),
                  sa_family_t addressFamily_ = 0,
                  std::string ipAddress_ = std::string());

    std::string interfaceName;
    sa_family_t addressFamily;
    std::string ipAddress;

} InterfaceInfo;


using InterfaceInfoList = std::list<InterfaceInfo>;
using ProtocolFamilySet = std::set<uint8_t>;


class SocketUtils
{
public:
    static bool initializeSockets();
    static void finalizeSockets();
    static bool checkSocketsInitialization();

    static bool isIpAddressString(const std::string &ipAddressString);

    static std::string sockAddrToString(const sockaddr_in &addr);
    static sockaddr_in ipAddressStringToAddress(const std::string &ipAddress,
                                                const uint16_t &port);

    static std::string hostToIpAddressString(const std::string &host);

#ifdef linux
    static InterfaceInfoList interfacesInfo(const std::string &adapterName = std::string(),
                                            const ProtocolFamilySet protocolFamilySet = {});
    static InterfaceInfo interfaceInfo(const std::string &adapterName,
                                       const uint8_t protocolFamily);
#endif

    static int lastError();
    static void closeSocket(Descriptor socket);
    static Descriptor createTcpServerSocket(uint16_t port,
                                            bool isBlocking,
                                            sockaddr_in &addr,
                                            int &errorCode);

    static Descriptor openTcpClientSocket(const std::string &ipAddress,
                                          const uint16_t port,
                                          int &errorCode,
                                          const int = 0);

    static Descriptor createUdpSocket(sockaddr_in &bindAddress,
                                      int &errorCode);

    static bool setBlockingMode(Descriptor socket,
                                bool isBlocking);

    static int inputBytesAvalable(const Descriptor descriptor);

private:
    static bool _isSocketsInitialized;
};

}
