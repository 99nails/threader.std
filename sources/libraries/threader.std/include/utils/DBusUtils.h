#pragma once

#include "ThreadsCommon.h"

#include <list>
#include <map>
#include <string>
#include <vector>

#include <dbus-1.0/dbus/dbus.h>
#include <systemd/sd-bus.h>

namespace Threader::Utils
{

constexpr char BUS_NAME_SYSTEMD[] = "org.freedesktop.systemd1";
constexpr char OBJECT_PATH_SYSTEMD[] = "/org/freedesktop/systemd1";
constexpr char INTERFACE_NAME_DBUS_PROPERTIES[] = "org.freedesktop.DBus.Properties";
constexpr char INTERFACE_NAME_SYSTEMD_MANAGER[] = "org.freedesktop.systemd1.Manager";

constexpr char FUNCTION_NAME_GET[] = "Get";
constexpr char FUNCTION_NAME_GET_STATE[] = "GetState";
constexpr char FUNCTION_NAME_GET_UNIT[] = "GetUnit";
constexpr char FUNCTION_NAME_START_UNIT[] = "StartUnit";
constexpr char FUNCTION_NAME_STOP_UNIT[] = "StopUnit";

constexpr char BUS_NAME_NETWORKMANAGER[]              =   "org.freedesktop.NetworkManager";
constexpr char OBJECT_PATH_NETWORKMANAGER[]           =   "/org/freedesktop/NetworkManager";
constexpr char OBJECT_PATH_NETWORKMANAGER_SETTINGS[]  =   "/org/freedesktop/NetworkManager/Settings";
constexpr char INTERFACE_NAME_NETWORKMANAGER[]        =   "org.freedesktop.NetworkManager";
constexpr char INTERFACE_NAME_NETWORKMANAGER_SETTINGS[]=  "org.freedesktop.NetworkManager.Settings";
constexpr char INTERFACE_NAME_CONNECTION_ACTIVE[]     =   "org.freedesktop.NetworkManager.Connection.Active";
constexpr char INTERFACE_NAME_SETTINGS_CONNECTION[]   =   "org.freedesktop.NetworkManager.Settings.Connection";
constexpr char FUNCTION_NAME_GETDEVICEBYIPIFACE[]     =   "GetDeviceByIpIface";
constexpr char FUNCTION_NAME_ACTIVATE_CONNECTION[]    =   "ActivateConnection";
constexpr char FUNCTION_NAME_DEACTIVATE_CONNECTION[]  =   "DeactivateConnection";
constexpr char FUNCTION_NAME_RELOAD_NM[]              =   "Reload";
constexpr char FUNCTION_NAME_LIST_CONNECTIONS[]       =   "ListConnections";
constexpr char FUNCTION_NAME_ID[]                     =   "Id";

constexpr char PROPERTY_NAME_ACTIVECONNECTIONS[]      =   "ActiveConnections";
constexpr char PROPERTY_NAME_FILENAME[]               =   "Filename";
constexpr char PROPERTY_NAME_ACTIVE_STATE[]    =   "ActiveState";
constexpr char PROPERTY_NAME_SUB_STATE[]       =   "SubState";

class DBusConnectionWrapper
{
public:
    explicit DBusConnectionWrapper();
    ~DBusConnectionWrapper();

    bool initialize();
    void finalize();

    bool isInitialized() const;
    DBusError *error();
    DBusConnection *connection() const;
    void registerError();
    void clearError();
    const std::string &lastError() const;

private:
    DBusError _error;
    DBusConnection *_connection;

    bool _isInitialized;
    std::string _lastError;
};

class DBusMethodWrapper
{
public:
    explicit DBusMethodWrapper(DBusConnectionWrapper *connection,
                               const char *busName,
                               const char *objectPath,
                               const char *interfaceName,
                               const char *methodName);
    ~DBusMethodWrapper();

    DBusConnectionWrapper *connection() const;
    const char *busName() const;
    const char *objectPath() const;
    const char *interfaceName() const;
    const char *methodName() const;
    bool initialize();
    void finalize();
    bool isInitialized() const;

protected:
    void checkError(const bool &hasError);

    bool appendArgs(int firstArgType, ...);
    bool getArgs(int firstArgType, ...);
    bool execute();

    bool extractStringFromVariantResult(std::string &result);

private:
    DBusConnectionWrapper *_connection;

    char *_busName {nullptr};
    char *_objectPath {nullptr};
    char *_interfaceName {nullptr};
    char *_methodName {nullptr};

    DBusMessage *_dBusMessage {nullptr};
    DBusMessage *_dBusReplyMessage {nullptr};
    bool _isInitialized;
};

class DBusMethodStartUnit : public DBusMethodWrapper
{
public:
    explicit DBusMethodStartUnit(DBusConnectionWrapper *connection);
    bool execute(const char *unitName, const char *reason);
    char *result() const;

private:
    char *_result = nullptr;
};

class DBusMethodStopUnit : public DBusMethodWrapper
{
public:
    explicit DBusMethodStopUnit(DBusConnectionWrapper *connection);
    bool execute(const char *unitName, const char *reason);
    char *result() const;

private:
    char *_result = nullptr;
};

class DBusMethodStopOrStartUnit : public DBusMethodWrapper
{
public:
    explicit DBusMethodStopOrStartUnit(DBusConnectionWrapper *connection, const bool start);
    bool execute(const char *unitName, const char *reason);
    char *result() const;

private:
    char *_result = nullptr;
};

class DBusMethodGetDeviceByIpIface : public DBusMethodWrapper
{
public:
    explicit DBusMethodGetDeviceByIpIface(DBusConnectionWrapper *connection);
    bool execute(const char *iface);
    char *result() const;

private:
    char *_result = nullptr;
};

class DBusMethodListConnections : public DBusMethodWrapper
{
public:
    explicit DBusMethodListConnections(DBusConnectionWrapper *connection);
    bool execute();

    const std::vector<std::string> &result() const;

private:
    std::vector<std::string> _result;
};

class DBusMethodActivateConnection : public DBusMethodWrapper
{
public:
    explicit DBusMethodActivateConnection(DBusConnectionWrapper *connection);
    bool execute(const char *networkConnection, const char *device, const char *specific);
    char *result() const;

private:
    char *_result = nullptr;
};

class DBusMethodDeactivateConnection : public DBusMethodWrapper
{
public:
    explicit DBusMethodDeactivateConnection(DBusConnectionWrapper *connection);
    bool execute(const char *activeConnection);
};

class DBusMethodReloadNetworkManager : public DBusMethodWrapper
{
public:
    explicit DBusMethodReloadNetworkManager(DBusConnectionWrapper *connection);
    bool execute(const uint32_t flags);
};

class DBusMethodGetUnitPath : public DBusMethodWrapper
{
public:
    explicit DBusMethodGetUnitPath(DBusConnectionWrapper *connection);
    bool execute(const char *unitName);
    char *result() const;

private:
    char *_result = nullptr;
};

class DBusMethodGetProperty : public DBusMethodWrapper
{
public:
    explicit DBusMethodGetProperty(DBusConnectionWrapper *connection,
                                   const std::string &busName,
                                   const std::string &objectPath);
    bool execute(const char *interface,
                 const char *propertyName);

    const std::string &value() const;

private:
    std::string _value;
};

enum class ServiceState {
    Unknown,
    Inactive,
    Activating,
    Active,
    Deactivating,
    Reloading,
    Failed,
    Maintenance
};

enum class ServiceSubState {
    Dead,
    Waiting,
    Running,
    Failed,
    Unknown
};


typedef struct {
    std::string unit;
    std::string path;
    std::string activeState;
    std::string subState;
    std::string description;
    std::string statusText;
    uint32_t mainPID;
} SystemDServiceState;

constexpr char INTERFACE_UNIT[]   = "org.freedesktop.systemd1.Unit";
constexpr char INTERFACE_SERVICE[] = "org.freedesktop.systemd1.Service";


class DBusUtils
{
public:
    static ServiceSubState getServiceSubStateFromDescription(const std::string &desc);

    static bool getProperty(const std::string &path,
                            const std::string &interface,
                            const std::string &name,
                            std::string &value);

    static bool getServicePath(const std::string &serviceName,
                               std::string &servicePath);

    static bool getServiceState(const std::string &serviceName,
                                std::string &serviceState,
                                std::string &serviceSubState,
                                std::string &serviceLoadState);

    static bool getUnitPath(const std::string &unitName,
                            std::string &unitPath);

    static bool getUnitState(const std::string &unitName,
                             std::string &unitState,
                             std::string &unitSubState);

    static bool checkServiceViaDBus(const std::string &serviceName);

    static std::string systemdGetUnitPath(sd_bus *bus, const std::string &name);

private:
    static DBusConnectionWrapper _dBusConnection;
    static std::string _lastDBusError;
    static std::map<std::string,std::string> _unitPath;
};

class SystemDUtils
{
public:
    static bool getUnitPath(sd_bus *bus,
                            const std::string &name,
                            std::string &unitPath);

    static bool getPropertyUInt32(sd_bus *bus,
                                  const char *path,
                                  const char *interface,
                                  const char *propertyName,
                                  uint32_t &value);

    static bool getPropertyString(sd_bus *bus, const char *path,
                                  const char *iface, const char *prop,
                                  std::string &value);

    static std::string find_unit_path(sd_bus *bus, const std::string &name);

    static bool getServiceState(const std::string &unit, SystemDServiceState &state);
};


}
