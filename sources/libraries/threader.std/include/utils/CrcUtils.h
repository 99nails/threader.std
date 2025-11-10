#pragma once

// #include <QCryptographicHash>
// #include <QtGlobal>

#include <cstdint>

#ifdef WIN32
#include <cstdint>
using u_int16_t = uint16_t;
#endif

#ifdef linux
// #include <stdlib.h>
#endif


namespace Threader::Utils {

class CrcUtils
{
public:
    static uint8_t Crc8(const unsigned char *pcBlock,
                              unsigned char len);

    static uint16_t Crc16(uint16_t start,
                          const unsigned char *data,
                          const int count);

    static uint16_t Crc16(uint8_t *data,
                          uint16_t length);

    static uint32_t Crc32(uint8_t *data, uint32_t length);

    // static QByteArray fileChecksum(const QString &fileName,
    //                                QCryptographicHash::Algorithm hashAlgorithm);

};

}
