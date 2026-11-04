#pragma once

#include "HandlerTcpSocket.h"
#include "ThreadsCommon.h"

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace Threader::Threads
{

/*!
 * \brief The HandlerSslSocket class - класс обработчика SSL соединения
 */
class HandlerSslSocket : public HandlerTcpSocket
{
public:
    /*!
     * \brief HandlerSslSocket - конструктор
     * \param host - адрес хоста соединения
     * \param port - порт соединения
     * \param descriptor - дескриптор соединения (по умолчанию - недопустимый дескриптор)
     * \param verifyCertificate - флаг необходимости проверки сертификата сервера (по умолчанию - false)
     * \param caCertificationFile - файл корневого сертификата центра авторизации (по умолчанию - пустая строка)
     * \param useExternalCertificates - флаг использования внешних (не из ресурсов) файлов сертификата и ключа сервера (по умолчанию - false)
     */
    explicit HandlerSslSocket(HandlerDataConsumer *consumer,
                              const std::string &host,
                              const uint16_t &port,
                              const Descriptor &descriptor = INVALID_DESCRIPTOR,
                              const bool verifyCertificate = false,
                              const std::string &certificateFilename = std::string(),
                              const std::string &primaryKeyFilename = std::string(),
                              const bool useExternalCertificates = false);

    /*!
     * \brief ~HandlerSslSocket - деструктор
     */
    ~HandlerSslSocket() override;

#ifdef linux
    /*!
     * \brief process - oбработка события голосования
     * \param event - pollfd структура голосующего
     * \return - признак обработки события
     */
    bool process(const pollfd &event) override;

    /*!
     * \brief assign - заполнение данными голосующего pollfd структуры,
     *  предоставляемой снаружи для формирования массива ожидания
     * \param event - pollfd структура голосующего
     */
    void assign(pollfd &event) override;
#endif

#ifdef WIN32
    /*!
     * \brief events - обработка событий на соединении
     * \return - вектор дескрипторов с событиями
     */
    DescriptorsVector *events() override;
#endif

    /*!
     * \brief close - закрыть соединение
     */
    void close() override;

    /*!
     * \brief read - чтение данных из соединения
     * \param data - указатель на буфер для чтения данных
     * \param count - размер буфера
     * \return - количество прочитанных данных
     */
    int read(void *data, int count) override;

    /*!
     * \brief write - запись данных в соединение
     * \param data - указатель на буфер данных для записи
     * \param count - размер буфера
     * \return - количество записанных данных
     */
    int write(const void *data, int count) override;

    /*!
     * \brief getCipherVersion - получить версию алгоритма шифрования
     * \return - строка, содержащая версию
     */
    std::string getCipherVersion();

    /*!
     * \brief getCipherName - получить имя алгоритма шифрования
     * \return -строка, содержащая имя
     */
    std::string getCipherName();

    /*!
     * \brief open - открыть соединение
     * \return - при удачном завершении true
     */
    bool open() override;

protected:
    /*!
     * \brief SSLInstance - создать или получить структуру SSL для соединения
     * \return - указатель на структуру
     */
    SSL* SSLInstance();

    /*!
     * \brief getClientContext - создать или получить SSL_CTX объект, содержащий
     * конфигурацию и соответствующие данные клиентского SSL соединения
     * \return - указатель на объект
     */
    static SSL_CTX* getClientContext();

    /*!
     * \brief getServerContext - создать или получить SSL_CTX объект, содержащий
     * конфигурацию и соответствующие данные серверного SSL соединения
     * \return - указатель на объект
     */
    /*static */SSL_CTX* getServerContext();

    /*!
     * \brief createInstance - создать структуру SSL и SSL_CTX объект (клиентский или серверный)
     * \return - true при успешном завершении
     */
    bool createInstance();

    /*!
     * \brief setInstanceDescriptor - установка дескриптора сокета в качестве средства ввода-вывода
     * для зашифрованной TLS системы SSL.
     * \return - true при успешном завершении
     */
    bool setInstanceDescriptor();

    /*!
     * \brief connect - запуск установки соединения TLS/SSL с сервером
     * \return - при успешном завершении возвращает 1.
     */
    int  connect();

    /*!
     * \brief accept - ожидание запуска клиентом установки TLS/SSL соединения
     * \return - при успешном установлении TLS/SSL соединения возвращает 1.
     */
    int  accept();

#ifdef linux
    /*!
     * \brief openTcpClientSocket - установка TCP соединения с сервером
     * \param ipAddress - адрес сервера
     * \param port - порт сервера
     * \param errorCode - ссылка на код ошибки при неудачном соединении
     * \return - дескриптор соединения (INVALID_DESCRIPTOR при ошибке)
     */
    Descriptor openTcpClientSocket(const std::string &ipAddress,
                                   const uint16_t port,
                                   int &errorCode);
#endif

#ifdef WIN32
    /*!
     * \brief openTcpClientSocket - установка TCP соединения с сервером
     * \param ipAddress - адрес сервера
     * \param port - порт сервера
     * \param errorCode - ссылка на код ошибки при неудачном соединении
     * \param timeout - задержка при проверке состояния дескриптора
     * \return - дескриптор соединения (INVALID_DESCRIPTOR при ошибке)
     */
    Descriptor openTcpClientSocket(const std::string &ipAddress,
                                   const uint16_t port,
                                   int &errorCode,
                                   const int &timeout = 0);
#endif

private:
    SSL* _SSLInstance;
    static SSL_CTX* _SSLClientContext;
    static SSL_CTX* _SSLServerContext;
    static int _clientContextRefCount;
    static int _serverContextRefCount;
    static bool _useExtrenalCerts;
    bool _verifyCertificate;
    std::string _certificateFilename;
    std::string _privateKeyFilename;

#ifdef WIN32
    bool _acceptInProcess;
    int  _acceptCounter;

    void processAccept();
    void doHandlerError(const int error = -1);
#endif
};

}

