#include "DBusUtils.h"
#include "StdStringUtils.h"

#include <iostream>
#include <cstring>

namespace Threader::Utils
{

/**
 * DBusConnection
 */

DBusConnectionWrapper::DBusConnectionWrapper()
    : _connection(nullptr)
    , _isInitialized(false)
    , _lastError(std::string())
{
}

DBusConnectionWrapper::~DBusConnectionWrapper()
{
    if (_isInitialized)
        finalize();
}

bool DBusConnectionWrapper::initialize()
{
    _isInitialized = false;

    // установка ссылки на структуру ошибок
    ::dbus_error_init(&_error);

    // подключение к D-Bus
    _connection = ::dbus_bus_get(DBUS_BUS_SYSTEM, &_error);
    _isInitialized = _connection;

    // регистрация ошибки
    if (!_isInitialized)
        registerError();
    else
        clearError();

    return _isInitialized;
}

void DBusConnectionWrapper::finalize()
{
    if (_connection)
    {
        ::dbus_connection_unref(_connection);
        _connection = nullptr;
    }

    _isInitialized = false;
}

bool DBusConnectionWrapper::isInitialized() const
{
    return _isInitialized;
}

DBusError *DBusConnectionWrapper::error()
{
    return &_error;
}

DBusConnection *DBusConnectionWrapper::connection() const
{
    return _connection;
}

void DBusConnectionWrapper::registerError()
{
    _lastError =  std::string(_error.name) + ": " + std::string(_error.message);
    dbus_error_free(&_error);
}

void DBusConnectionWrapper::clearError()
{
    _lastError = std::string();
}

const std::string &DBusConnectionWrapper::lastError() const
{
    return _lastError;
}

/**
 * DBusMethod
 */

DBusMethodWrapper::DBusMethodWrapper(DBusConnectionWrapper *connection,
                                     const char *busName,
                                     const char *objectPath,
                                     const char *interfaceName,
                                     const char *methodName)
    : _connection(connection)
    , _isInitialized(false)
{
    auto len = std::strlen(busName) + 1;
    _busName = new char[len];
    std::strncpy(_busName, busName, len);

    len = std::strlen(objectPath) + 1;
    _objectPath = new char[len];
    std::strncpy(_objectPath, objectPath, len);

    len = std::strlen(interfaceName) + 1;
    _interfaceName = new char[len];
    std::strncpy(_interfaceName, interfaceName, len);

    len = std::strlen(methodName) + 1;
    _methodName = new char[len];
    std::strncpy(_methodName, methodName, len);
}

DBusMethodWrapper::~DBusMethodWrapper()
{
    if (_busName)
    {
        delete [] _busName;
        _busName = nullptr;
    }

    if (_objectPath)
    {
        delete [] _objectPath;
        _objectPath = nullptr;
    }

    if (_interfaceName)
    {
        delete [] _interfaceName;
        _interfaceName = nullptr;
    }

    if (_methodName)
    {
        delete [] _methodName;
        _methodName = nullptr;
    }
}

DBusConnectionWrapper *DBusMethodWrapper::connection() const
{
    return _connection;
}

const char *DBusMethodWrapper::busName() const
{
    return _busName;
}

const char *DBusMethodWrapper::objectPath() const
{
    return _objectPath;
}

const char *DBusMethodWrapper::interfaceName() const
{
    return _interfaceName;
}

const char *DBusMethodWrapper::methodName() const
{
    return _methodName;
}

bool DBusMethodWrapper::initialize()
{
    _dBusMessage = ::dbus_message_new_method_call(_busName,
                                                  _objectPath,
                                                  _interfaceName,
                                                  _methodName);
    _isInitialized = _dBusMessage;
    return _isInitialized;
}

void DBusMethodWrapper::finalize()
{
    if (_dBusMessage)
    {
        ::dbus_message_unref(_dBusMessage);
        _dBusMessage = nullptr;
    }

    if (_dBusReplyMessage)
    {
        ::dbus_message_unref(_dBusReplyMessage);
        _dBusReplyMessage = nullptr;
    }

    _isInitialized = false;
}

bool DBusMethodWrapper::isInitialized() const
{
    return _isInitialized;
}

void DBusMethodWrapper::checkError(const bool &hasError)
{
    if (_connection)
    {
        if (hasError)
        {
            _connection->registerError();
        }
        else
            _connection->clearError();
    }
}

bool DBusMethodWrapper::appendArgs(int firstArgType, ...)
{
    va_list args;
    va_start(args, firstArgType);
    auto result = dbus_message_append_args_valist(_dBusMessage, firstArgType, args);
    va_end(args);

    checkError(!result);

    return result;
}

bool DBusMethodWrapper::getArgs(int firstArgType, ...)
{
    if (!_dBusReplyMessage || !_connection)
        return false;

    va_list args;
    va_start(args, firstArgType);
    auto result = dbus_message_get_args_valist(_dBusReplyMessage,
                                               connection()->error(),
                                               firstArgType,
                                               args);
    va_end(args);

    checkError(!result);

    return result;
}

bool DBusMethodWrapper::execute()
{
    if(!_connection)
        return false;

    if(_dBusReplyMessage)
    {
        ::dbus_message_unref(_dBusReplyMessage);
        _dBusReplyMessage = nullptr;
    }

    _dBusReplyMessage = ::dbus_connection_send_with_reply_and_block(_connection->connection(),
                                                                    _dBusMessage,
                                                                    DBUS_TIMEOUT_USE_DEFAULT,
                                                                    _connection->error());
    bool result = _dBusReplyMessage;
    checkError(!result);

    return result;
}

static std::string extractStringFromVariant(DBusMessage *reply, DBusError *error)
{
    DBusMessageIter iter, array_iter;
    DBusMessageIter sub;
    char *str(nullptr);

    dbus_message_iter_init(reply, &iter);

    if (DBUS_TYPE_VARIANT != dbus_message_iter_get_arg_type(&iter))
    {
        dbus_set_error_const(error, "reply_should_be_variant", "This message hasn't a variant response type");
        return 0;
    }

    dbus_message_iter_recurse(&iter, &sub);

    auto type = dbus_message_iter_get_arg_type(&sub);

    switch(type)
    {
    case DBUS_TYPE_STRING:
        dbus_message_iter_get_basic(&sub, &str);
        return std::string(str);
        // break;

    case DBUS_TYPE_ARRAY:
    {
        auto subType = dbus_message_iter_get_element_type(&sub);
        if (DBUS_TYPE_OBJECT_PATH != subType && DBUS_TYPE_STRING != subType)
        {
            dbus_set_error_const(error, "array_element_should_be_object_path_or_string",
                                 "This array reply message must have object path or string content");
            return 0;
        }

        dbus_message_iter_recurse(&sub, &array_iter);
        std::string result("");

        while (DBUS_TYPE_INVALID != dbus_message_iter_get_arg_type(&array_iter))
        {
            str = nullptr;
            dbus_message_iter_get_basic(&array_iter, &str);
            result += std::string(str) + " ";
            dbus_message_iter_next(&array_iter);
        }

        if (!result.empty())
            result.erase(result.length() - 1, 1);

        return result;
    }
    default:
        dbus_set_error_const(error,
                             "variant_should_be_string_or_array",
                             "This variant reply message must have string or array content");
        return 0;
        // break;
    }

    //    if (DBUS_TYPE_STRING != type) {
    //        dbus_set_error_const(error, "variant_should_be_double", "This variant reply message must have double content");
    //        return 0;
    //    }

    //    dbus_message_iter_get_basic(&sub, &str);
    //    return std::string(str);
}

bool DBusMethodWrapper::extractStringFromVariantResult(std::string &result)
{
    if (!_connection)
        return false;

    result = extractStringFromVariant(_dBusReplyMessage, _connection->error());
    return !result.empty();
}

/**
 * DBusMethodStartUnit
 */

DBusMethodStartUnit::DBusMethodStartUnit(DBusConnectionWrapper *connection)
    :DBusMethodWrapper(connection,
                       BUS_NAME_SYSTEMD,
                       OBJECT_PATH_SYSTEMD,
                       INTERFACE_NAME_SYSTEMD_MANAGER,
                       FUNCTION_NAME_START_UNIT)
{
}

bool DBusMethodStartUnit::execute(const char *unitName, const char *reason)
{
    if (appendArgs(DBUS_TYPE_STRING, &unitName,
                   DBUS_TYPE_STRING, &reason,
                   DBUS_TYPE_INVALID))
    {
        auto res = DBusMethodWrapper::execute();
        if (res)
        {
            if (getArgs(DBUS_TYPE_OBJECT_PATH, &_result,
                        DBUS_TYPE_INVALID))
            {
                return res;
            }
        }
    }

    return false;
}

char *DBusMethodStartUnit::result() const
{
    return _result;
}

/**
 * DBusMethodStopUnit
 */

DBusMethodStopUnit::DBusMethodStopUnit(DBusConnectionWrapper *connection)
    :DBusMethodWrapper(connection,
                       BUS_NAME_SYSTEMD,
                       OBJECT_PATH_SYSTEMD,
                       INTERFACE_NAME_SYSTEMD_MANAGER,
                       FUNCTION_NAME_STOP_UNIT)
{
}

bool DBusMethodStopUnit::execute(const char *unitName,
                                 const char *reason)
{
    if (appendArgs(DBUS_TYPE_STRING, &unitName,
                   DBUS_TYPE_STRING, &reason,
                   DBUS_TYPE_INVALID))
    {
        auto res = DBusMethodWrapper::execute();
        if (res)
        {
            if (getArgs(DBUS_TYPE_OBJECT_PATH, &_result,
                        DBUS_TYPE_INVALID))
            {
                return res;
            }
        }
    }

    return false;
}

char *DBusMethodStopUnit::result() const
{
    return _result;
}

/**
 * DBusMethodStopOrStartUnit
 */

DBusMethodStopOrStartUnit::DBusMethodStopOrStartUnit(DBusConnectionWrapper *connection, const bool start)
    : DBusMethodWrapper(connection,
                        BUS_NAME_SYSTEMD,
                        OBJECT_PATH_SYSTEMD,
                        INTERFACE_NAME_SYSTEMD_MANAGER,
                        start ? FUNCTION_NAME_START_UNIT : FUNCTION_NAME_STOP_UNIT)
{
}

bool DBusMethodStopOrStartUnit::execute(const char *unitName, const char *reason)
{
    if (appendArgs(DBUS_TYPE_STRING, &unitName,
                   DBUS_TYPE_STRING, &reason,
                   DBUS_TYPE_INVALID))
    {
        auto res = DBusMethodWrapper::execute();
        if (res)
        {
            if (getArgs(DBUS_TYPE_OBJECT_PATH, &_result,
                        DBUS_TYPE_INVALID))
            {
                return res;
            }
        }
    }

    return false;
}

char *DBusMethodStopOrStartUnit::result() const
{
    return _result;
}

/**
 * DBusMethodGetDeviceByIpIface
 */
DBusMethodGetDeviceByIpIface::DBusMethodGetDeviceByIpIface(DBusConnectionWrapper *connection) :
    DBusMethodWrapper(connection,
                      BUS_NAME_NETWORKMANAGER,
                      OBJECT_PATH_NETWORKMANAGER,
                      INTERFACE_NAME_NETWORKMANAGER,
                      FUNCTION_NAME_GETDEVICEBYIPIFACE)
{
}

bool DBusMethodGetDeviceByIpIface::execute(const char *iface)
{
    if(appendArgs(DBUS_TYPE_STRING, &iface,
                  DBUS_TYPE_INVALID))
    {
        auto res = DBusMethodWrapper::execute();
        if(res)
            if(getArgs(DBUS_TYPE_OBJECT_PATH, &_result,
                       DBUS_TYPE_INVALID))
                return res;
    }

    return false;
}

char *DBusMethodGetDeviceByIpIface::result() const
{
    return _result;
}

/**
 * @brief DBusMethodListConnections
 */
DBusMethodListConnections::DBusMethodListConnections(DBusConnectionWrapper *connection) :
    DBusMethodWrapper(connection,
                      BUS_NAME_NETWORKMANAGER,
                      OBJECT_PATH_NETWORKMANAGER_SETTINGS,
                      INTERFACE_NAME_NETWORKMANAGER_SETTINGS,
                      FUNCTION_NAME_LIST_CONNECTIONS)
{
}

bool DBusMethodListConnections::execute()
{
    if(appendArgs(DBUS_TYPE_INVALID))
    {
        auto res = DBusMethodWrapper::execute();
        if(res)
        {
            char **retVal;
            int32_t numArgs;
            if(getArgs(DBUS_TYPE_ARRAY, DBUS_TYPE_OBJECT_PATH, &retVal, &numArgs, DBUS_TYPE_INVALID))
            {
                auto data = std::string("");
                if(numArgs > 0)
                {
                    for(int32_t i = 0; i < numArgs;i++)
                        data += std::string(retVal[i]) + " ";
                    data.erase(_result.size() - 1, 1);

                }
                _result = StdStringUtils::split(data, ' ', true);
                dbus_free_string_array(retVal);
                return res;
            }
        }
    }

    return false;
}

const std::vector<std::string> &DBusMethodListConnections::result() const
{
    return _result;
}

/**
 * @brief DBusMethodActivateConnection
 */
DBusMethodActivateConnection::DBusMethodActivateConnection(DBusConnectionWrapper *connection) :
    DBusMethodWrapper(connection,
                      BUS_NAME_NETWORKMANAGER,
                      OBJECT_PATH_NETWORKMANAGER,
                      INTERFACE_NAME_NETWORKMANAGER,
                      FUNCTION_NAME_ACTIVATE_CONNECTION)
{
}

bool DBusMethodActivateConnection::execute(const char *networkConnection, const char *device, const char *specific)
{
    if(appendArgs(DBUS_TYPE_OBJECT_PATH, &networkConnection,
                  DBUS_TYPE_OBJECT_PATH, &device,
                  DBUS_TYPE_OBJECT_PATH, &specific,
                  DBUS_TYPE_INVALID))
    {
        auto res = DBusMethodWrapper::execute();
        if(res)
            if(getArgs(DBUS_TYPE_OBJECT_PATH, &_result,
                       DBUS_TYPE_INVALID))
                return res;
    }

    return false;
}

char *DBusMethodActivateConnection::result() const
{
    return _result;
}

/**
 * @brief DBusMethodDeactivateConnection
 * @param connection
 */
DBusMethodDeactivateConnection::DBusMethodDeactivateConnection(DBusConnectionWrapper *connection) :
    DBusMethodWrapper(connection,
                      BUS_NAME_NETWORKMANAGER,
                      OBJECT_PATH_NETWORKMANAGER,
                      INTERFACE_NAME_NETWORKMANAGER,
                      FUNCTION_NAME_DEACTIVATE_CONNECTION)
{
}

bool DBusMethodDeactivateConnection::execute(const char *activeConnection)
{
    if(appendArgs(DBUS_TYPE_STRING, &activeConnection,
                  DBUS_TYPE_INVALID))
    {
        auto result = DBusMethodWrapper::execute();
        if(result)
            return true;
    }

    return false;
}

/**
 * @brief DBusMethodReloadNetworkManager
 */
DBusMethodReloadNetworkManager::DBusMethodReloadNetworkManager(DBusConnectionWrapper *connection) :
    DBusMethodWrapper(connection,
                      BUS_NAME_NETWORKMANAGER,
                      OBJECT_PATH_NETWORKMANAGER,
                      INTERFACE_NAME_NETWORKMANAGER,
                      FUNCTION_NAME_RELOAD_NM)
{
}

bool DBusMethodReloadNetworkManager::execute(const uint32_t flags)
{
    bool result(false);

    if(appendArgs(DBUS_TYPE_UINT32, &flags,
                  DBUS_TYPE_INVALID))
        result = DBusMethodWrapper::execute();

    return result;
}

/**
 * DBusMethodGetUnitPath
 */

DBusMethodGetUnitPath::DBusMethodGetUnitPath(DBusConnectionWrapper *connection)
    : DBusMethodWrapper(connection,
                        BUS_NAME_SYSTEMD,
                        OBJECT_PATH_SYSTEMD,
                        INTERFACE_NAME_SYSTEMD_MANAGER,
                        FUNCTION_NAME_GET_UNIT)

{
}

bool DBusMethodGetUnitPath::execute(const char *unitName)
{
    if (appendArgs(DBUS_TYPE_STRING, &unitName,
                   DBUS_TYPE_INVALID))
    {
        if (!DBusMethodWrapper::execute())
            return false;

        return getArgs(DBUS_TYPE_OBJECT_PATH, &_result,
                       DBUS_TYPE_INVALID);
    }
    return false;
}

char *DBusMethodGetUnitPath::result() const
{
    return _result;
}

/**
 * DBusMethodGetProperty
 */
DBusMethodGetProperty::DBusMethodGetProperty(DBusConnectionWrapper *connection,
                                             const std::string &busName,
                                             const std::string &objectPath)
    : DBusMethodWrapper(connection,
                        busName.c_str(),
                        objectPath.c_str(),
                        INTERFACE_NAME_DBUS_PROPERTIES,
                        FUNCTION_NAME_GET)
    , _value(std::string())
{
}

bool DBusMethodGetProperty::execute(const char *interface,
                                    const char *propertyName)
{
    if (appendArgs(DBUS_TYPE_STRING, &interface,
                   DBUS_TYPE_STRING, &propertyName,
                   DBUS_TYPE_INVALID))
    {
        if(!DBusMethodWrapper::execute())
            return false;

        return extractStringFromVariantResult(_value);
    }

    return false;
}

const std::string &DBusMethodGetProperty::value() const
{
    return _value;
}

DBusConnectionWrapper DBusUtils::_dBusConnection = DBusConnectionWrapper();

std::string DBusUtils::_lastDBusError = std::string();
std::map<std::string,std::string> DBusUtils::_unitPath = {};

constexpr char SERVICE_NAME_SUFFIX[] = ".service";
constexpr char INTERFACE_NAME_UNIT[] = "org.freedesktop.systemd1.Unit";
constexpr char PROPERTY_NAME_LOAD_STATE[] = "LoadState";
//constexpr char PROPERTY_NAME_ACTIVE_STATE[] = "ActiveState";
//constexpr char PROPERTY_NAME_SUB_STATE[] = "SubState";

constexpr char OPERATION_NAME_GET_UNIT_PATH[] = "Получение пути к модулю %s";
constexpr char OPERATION_NAME_GET_PROPERTY[]  = "Получение свойства %s, путь %s, интерфейс %s";
constexpr char OPERATION_NAME_STOP_SERVICE[]  = "Остановка службы %s";
constexpr char OPERATION_NAME_START_SERVICE[] = "Запуск службы %s";


ServiceSubState DBusUtils::getServiceSubStateFromDescription(const std::string &desc)
{
    if(desc == "deed")
        return ServiceSubState::Dead;
    else if(desc == "waiting")
        return ServiceSubState::Waiting;
    else if(desc == "running")
        return ServiceSubState::Running;
    else if(desc == "failed")
        return ServiceSubState::Failed;
    else
        return ServiceSubState::Unknown;
}

bool DBusUtils::getProperty(const std::string &path,
                            const std::string &interface,
                            const std::string &name,
                            std::string &value)
{
    std::string operationName = format(OPERATION_NAME_GET_PROPERTY, name.c_str(), path.c_str(), interface.c_str());
    //    writeLog(Message12106, STRLOG(operationName));

    DBusMethodGetProperty getPropertyDBase(&_dBusConnection, std::string(BUS_NAME_SYSTEMD), path);

    getPropertyDBase.initialize();

    if(!getPropertyDBase.isInitialized())
    {
        _lastDBusError = _dBusConnection.lastError();
        //        writeLog(Message12108, STRLOG2(operationName, _lastDBusError));
        return false;
    }

    auto result = getPropertyDBase.execute(interface.c_str(), name.c_str());

    if(result)
    {
        _lastDBusError.clear();
        //        writeLog(Message12107, STRLOG(operationName));
        value = getPropertyDBase.value();
    }
    else
    {
        _lastDBusError = _dBusConnection.lastError();
        //        writeLog(Message12108, STRLOG2(operationName, _lastDBusError));
    }

    return result;

}

bool DBusUtils::getServicePath(const std::string &serviceName,
                               std::string &servicePath)
{
    if(_unitPath.count(serviceName) > 0)
    {
        servicePath = _unitPath[serviceName];
        return true;
    }

    std::string operationName = format(OPERATION_NAME_GET_UNIT_PATH, serviceName.c_str());
    //    writeLog(Message12106, STRLOG(operationName));

    DBusMethodGetUnitPath getUnitPathDbus(&_dBusConnection);

    getUnitPathDbus.initialize();

    if(!getUnitPathDbus.isInitialized())
    {
        _lastDBusError = _dBusConnection.lastError();
        //        writeLog(Message12108, STRLOG2(operationName, _lastDBusError));
        return false;
    }

    auto result = getUnitPathDbus.execute(serviceName.c_str());

    if(result)
    {
        _lastDBusError.clear();
        //        writeLog(Message12107, STRLOG(operationName));
        servicePath = getUnitPathDbus.result();
        _unitPath[serviceName] = servicePath;
    }
    else
    {
        _lastDBusError = _dBusConnection.lastError();
        //        writeLog(Message12108, STRLOG2(operationName, _lastDBusError));
    }

    return result;

}

bool DBusUtils::getServiceState(const std::string &serviceName,
                                std::string &serviceState,
                                std::string &serviceSubState,
                                std::string &serviceLoadState)
{
    std::string serviceNameWithSuffix = StdStringUtils::toLower(serviceName);
    if(!StdStringUtils::endsWith(serviceNameWithSuffix, SERVICE_NAME_SUFFIX))
        serviceNameWithSuffix += SERVICE_NAME_SUFFIX;

    std::string servicePath;
    bool result = getServicePath(serviceNameWithSuffix, servicePath);

    if(!result || servicePath.empty())
        return false;

    //    writeLog(Message12109, STRLOG2(serviceName, servicePath));

    result = getProperty(servicePath,
                         INTERFACE_NAME_UNIT,
                         PROPERTY_NAME_ACTIVE_STATE,
                         serviceState);
    if(!result || serviceName.empty())
        return false;

    result = getProperty(servicePath,
                         INTERFACE_NAME_UNIT,
                         PROPERTY_NAME_SUB_STATE,
                         serviceSubState);
    if(!result || serviceSubState.empty())
        return false;

    result = getProperty(servicePath,
                         INTERFACE_NAME_UNIT,
                         PROPERTY_NAME_LOAD_STATE,
                         serviceLoadState);
    if(!result || serviceLoadState.empty())
        return false;

    //    writeLog(Message12110, STRLOG3(serviceName, serviceLoadState, serviceState), STRLOG(serviceSubState));

    _lastDBusError.clear();
    return true;
}

bool DBusUtils::getUnitPath(const std::string &unitName, std::string &unitPath)
{
    if (_unitPath.count(unitName) >0)
    {
        unitPath = _unitPath[unitName];
        return true;
    }

    //    std::string operationName = format(OPERATION_NAME_GET_UNIT_PATH, unitName.c_str());
    //    writeLog(Message100, STRLOG(operationName));

    DBusMethodGetUnitPath getUnitPathDbus(&_dBusConnection);

    if (!getUnitPathDbus.isInitialized())
        getUnitPathDbus.initialize();

    if (!getUnitPathDbus.isInitialized())
    {
        //        writeLog(Message102, STRLOG2(operationName, _dBusConnection->lastError()));
        return false;
    }

    auto result = getUnitPathDbus.execute(unitName.c_str());

    if (result)
    {
        //        writeLog(Message101, STRLOG(operationName));
        unitPath = std::string(getUnitPathDbus.result());
        _unitPath[unitName] = unitPath;
    }
    //    else
    //        writeLog(Message102, STRLOG2(operationName, _dBusConnection->lastError()));

    return result;

}

bool DBusUtils::getUnitState(const std::string &unitName, std::string &unitState, std::string &unitSubState)
{
    std::string unitPath = std::string();
    bool result = getUnitPath(unitName, unitPath);

    if (!result ||  unitPath.empty())
        return false;

    //    writeLog(Message110, STRLOG(unitName), STRLOG((unitPath)));

    result = getProperty(unitPath,
                         INTERFACE_NAME_UNIT,
                         PROPERTY_NAME_ACTIVE_STATE,
                         unitState);

    if (!result || unitState.empty())
        return false;

    result = getProperty(unitPath,
                         INTERFACE_NAME_UNIT,
                         PROPERTY_NAME_SUB_STATE,
                         unitSubState);
    if (!result || unitSubState.empty())
        return false;

    //    writeLog(Message111, STRLOG3(unitName, unitState, unitSubState));

    return true;

}

bool DBusUtils::checkServiceViaDBus(const std::string &serviceName)
{
    if (!_dBusConnection.isInitialized())
        _dBusConnection.initialize();

    if (!_dBusConnection.isInitialized())
        return false;

    std::string serviceNameWithSuffix = serviceName;
    if (!StdStringUtils::endsWith(serviceNameWithSuffix, SERVICE_NAME_SUFFIX))
        serviceNameWithSuffix += SERVICE_NAME_SUFFIX;

    std::string unitState, unitSubState;
    //    ServiceState state(ServiceState::Unknown);
    ServiceSubState subState{ServiceSubState::Unknown};
    //    auto started = DateTimeUtils::getTickCount();
    while(1)
    {
        if (!getUnitState(serviceNameWithSuffix, unitState, unitSubState))
            return false;

        subState = getServiceSubStateFromDescription(StdStringUtils::toLower(unitSubState));
        if(subState != ServiceSubState::Waiting)
            break;

        //        if(started + TIMEOUT_CHECK_SERVICE <= DateUtils::getTickCount())
        //            return false;

        //        DateUtils::sleepMilliseconds(100);
    }

    if (subState == ServiceSubState::Running)
    {
        //        writeLog(Message37, STRLOG(serviceName));
        return true;
    }

    //    writeLog(Message7, STRLOG(serviceName));

    //    if (startUnit(serviceNameWithSuffix))
    //    {
    //        writeLog(Message36, STRLOG(serviceName));
    //        return true;
    //    }

    //    writeLog(Message35, STRLOG(serviceName));
    return false;
}

bool SystemDUtils::getUnitPath(sd_bus *bus,
                               const std::string &name,
                               std::string &unitPath)
{
    sd_bus_message *reply = nullptr;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    const char *path = nullptr;

    int r = sd_bus_call_method(bus,
                               "org.freedesktop.systemd1",
                               "/org/freedesktop/systemd1",
                               "org.freedesktop.systemd1.Manager",
                               "GetUnit",
                               &error,
                               &reply,
                               "s",
                               name.c_str()
                               );

    if (r < 0)
    {
        sd_bus_error_free(&error);
        return false;
    }

    r = sd_bus_message_read(reply, "o", &path);
    unitPath = (r >= 0 && path) ? path : "";
    sd_bus_message_unref(reply);
    sd_bus_error_free(&error);
    return true;
}

bool SystemDUtils::getPropertyUInt32(sd_bus *bus,
                                     const char *path,
                                     const char *interface,
                                     const char *propertyName,
                                     uint32_t &value)
{
    sd_bus_error error = SD_BUS_ERROR_NULL;
    uint32_t val = 0;

    int r = sd_bus_get_property_trivial(bus,
                                        "org.freedesktop.systemd1",
                                        path,
                                        interface,
                                        propertyName,
                                        &error,
                                        'u',
                                        &value);

    sd_bus_error_free(&error);
    return (r >= 0);

}

bool SystemDUtils::getPropertyString(sd_bus *bus,
                                     const char *path,
                                     const char *iface,
                                     const char *prop,
                                     std::string &value)
{
    sd_bus_error error = SD_BUS_ERROR_NULL;
    char *val = nullptr;

    int r = sd_bus_get_property_string(bus,
                                       "org.freedesktop.systemd1",
                                       path,
                                       iface,
                                       prop,
                                       &error,
                                       &val);

    if (r < 0)
    {
        value = error.message ? error.message : strerror(-r);
        sd_bus_error_free(&error);
        return false;
    }

    value = val ? val : "(null)";
    free(val);
    sd_bus_error_free(&error);
    return true;
}


std::string SystemDUtils::find_unit_path(sd_bus *bus, const std::string &name)
{
    sd_bus_message *reply = nullptr;
    sd_bus_error error = SD_BUS_ERROR_NULL;

    // ListUnits принимает фильтр (prefix) и возвращает массив структур:
    // [ (name, description, load_state, active_state, sub_state, object_path) ]
    int r = sd_bus_call_method(
                bus,
                "org.freedesktop.systemd1",
                "/org/freedesktop/systemd1",
                "org.freedesktop.systemd1.Manager",
                "ListUnits",
                &error,
                &reply,
                "s",
                name.c_str()  // префикс: ищем по имени (лучше "sshd.service", а не просто "sshd")
                );

    if (r < 0) {
        std::cerr << "ListUnits failed: " << (error.message ? error.message : strerror(-r)) << "\n";
        sd_bus_error_free(&error);
        return "";
    }

    // Читаем массив структур
    r = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "(ssssso)");
    if (r < 0)
    {
        sd_bus_message_unref(reply);
        sd_bus_error_free(&error);
        return "";
    }

    while ((r = sd_bus_message_read(reply, "(ssssso)", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)) > 0)
    {
        // Нам нужен только последний элемент — object_path
        const char *path = nullptr;
        r = sd_bus_message_peek_type(reply, nullptr, nullptr);
        if (r >= 0) {
            r = sd_bus_message_read(reply, "o", &path);
            if (r >= 0 && path) {
                sd_bus_message_exit_container(reply); // выходим из массива
                sd_bus_message_unref(reply);
                sd_bus_error_free(&error);
                return std::string(path);
            }
        }
    }

    sd_bus_message_exit_container(reply);
    sd_bus_message_unref(reply);
    sd_bus_error_free(&error);
    return "";
}

// Поиск юнита через ListUnits (без аргументов!)
// Возвращает {object_path, load_state, active_state, sub_state}
struct UnitInfo {
    std::string path;
    std::string load_state;
    std::string active_state;
    std::string sub_state;
};

static UnitInfo find_unit(sd_bus *bus, const std::string &name)
{
    UnitInfo info;
    sd_bus_message *reply = nullptr;
    sd_bus_error error = SD_BUS_ERROR_NULL;

    // ListUnits: входных аргументов НЕТ (сигнатура "")
    int r = sd_bus_call_method(
        bus,
        "org.freedesktop.systemd1",
        "/org/freedesktop/systemd1",
        "org.freedesktop.systemd1.Manager",
        "ListUnits",
        &error,
        &reply,
        ""          // ← пустая сигнатура, никаких аргументов
    );

    if (r < 0) {
        std::cerr << "ListUnits: " << (error.message ? error.message : strerror(-r)) << "\n";
        sd_bus_error_free(&error);
        return info;
    }

    // Возвращается массив структур: a(ssssssouso)
    // Поля: name, description, load_state, active_state, sub_state,
    //        following, object_path, job_id, job_type, job_path
    r = sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "(ssssssouso)");
    if (r < 0) {
        sd_bus_message_unref(reply);
        sd_bus_error_free(&error);
        return info;
    }

    while (true) {
        r = sd_bus_message_enter_container(reply, SD_BUS_TYPE_STRUCT, "ssssssouso");
        if (r <= 0)
            break;

        const char *unit_name = nullptr;
        const char *desc       = nullptr;
        const char *load       = nullptr;
        const char *active     = nullptr;
        const char *sub        = nullptr;
        const char *following  = nullptr;
        const char *obj_path   = nullptr;

        r = sd_bus_message_read(reply, "ssssssouso",
            &unit_name, &desc, &load, &active, &sub,
            &following, &obj_path, nullptr, nullptr, nullptr);

        if (r < 0)
            break;

        std::cout << unit_name <<std::endl;

        if (unit_name && name == unit_name) {
            info.path         = obj_path   ? obj_path   : "";
            info.load_state   = load       ? load       : "";
            info.active_state = active     ? active     : "";
            info.sub_state    = sub        ? sub        : "";
            sd_bus_message_exit_container(reply);
            break;
        }

        sd_bus_message_exit_container(reply);
    }

    sd_bus_message_exit_container(reply);
    sd_bus_message_unref(reply);
    sd_bus_error_free(&error);
    return info;
}

bool SystemDUtils::getServiceState(const std::string &unit,
                                   SystemDServiceState &state)
{
    state = {};

    sd_bus *bus = nullptr;
    bool result = sd_bus_open_system(&bus) >= 0;
    std::string path;

    if (!result)
    {
        sd_bus_unref(bus);
        return result;
    }

//    auto info = find_unit(bus, unit);

//    if (path.empty()) {
//        sd_bus_unref(bus);
//        return 1;
//    }

    result = getUnitPath(bus, unit, path);
    if (!result)
        goto finalization;

    state.unit = unit;
    state.path = path;

    result = getPropertyString(bus, path.c_str(), INTERFACE_UNIT, "ActiveState", state.activeState);
    result &= getPropertyString(bus, path.c_str(), INTERFACE_UNIT, "SubState", state.subState);
    result &= getPropertyString(bus, path.c_str(), INTERFACE_UNIT, "Description", state.description);
    result &= getPropertyString(bus, path.c_str(), INTERFACE_SERVICE, "StatusText", state.statusText);
    result &= getPropertyUInt32(bus, path.c_str(), INTERFACE_SERVICE, "MainPID", state.mainPID);

finalization:
    sd_bus_unref(bus);

    return result;
}

bool SystemDUtils::controlUnit(const std::string &unit, UnitControlMethod method)
{
    // Подключаемся к системной шине D-Bus
    sd_bus *bus = nullptr;
    bool result = sd_bus_open_system(&bus) >= 0;
    if (!result)
    {
        sd_bus_unref(bus);
        return result;
    }

    // Преобразуем enum -> std::string
    std::string strMethod;
    if (method == UnitControlMethod::Start)
        strMethod = "StartUnit";
    else if (method == UnitControlMethod::Stop)
        strMethod = "StopUnit";
    else if (method == UnitControlMethod::Restart)
        strMethod = "RestartUnit";
    else
        return false;

    // Вызываем метод остановки сервиса
    result = sd_bus_call_method(bus,
                                "org.freedesktop.systemd1",         // сервис
                                "/org/freedesktop/systemd1",        // путь
                                "org.freedesktop.systemd1.Manager", // интерфейс
                                strMethod.c_str(),                  // метод
                                nullptr,                            // объект для ошибок
                                nullptr,                            // ответ
                                "ss",                               // типы аргументов (string, string)
                                unit.c_str(),                       // имя службы
                                "replace"                           // режим замены
                                ) >= 0;

    // Очистка и завершение
    sd_bus_unref(bus);
    return result;
}

bool SystemDUtils::startUnit(const std::string &unit)
{
    return controlUnit(unit, UnitControlMethod::Start);
}

bool SystemDUtils::stopUnit(const std::string &unit)
{
    return controlUnit(unit, UnitControlMethod::Stop);
}

bool SystemDUtils::restartUnit(const std::string &unit)
{
    return controlUnit(unit, UnitControlMethod::Restart);
}
}
