#ifndef MACROCONTROLLER_H
#define MACROCONTROLLER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

#include "bluetoothhid.h"
#include "macroconfig.h"

/**
 * @brief MacroController - Main controller for the macro pad application
 * 
 * This class serves as the bridge between the QML UI and the C++ backend.
 * It manages macro execution and Bluetooth HID communication.
 */
class MacroController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(bool discoverable READ isDiscoverable WRITE setDiscoverable NOTIFY discoverableChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QVariantList macros READ macros NOTIFY macrosChanged)
    Q_PROPERTY(int columns READ columns NOTIFY columnsChanged)
    Q_PROPERTY(int rows READ rows NOTIFY rowsChanged)

public:
    explicit MacroController(QObject *parent = nullptr);
    ~MacroController();

    bool isConnected() const;
    bool isDiscoverable() const;
    QString status() const;
    QString deviceName() const;
    QVariantList macros() const;
    int columns() const;
    int rows() const;

    void setDiscoverable(bool discoverable);
    void setDeviceName(const QString &name);

public slots:
    /**
     * @brief Initialize the controller and Bluetooth
     */
    bool initialize();

    /**
     * @brief Execute a macro by ID
     */
    void executeMacro(const QString &macroId);

    /**
     * @brief Start Bluetooth pairing mode
     */
    void startPairing();

    /**
     * @brief Disconnect from current device
     */
    void disconnect();

    /**
     * @brief Reload macro configuration
     */
    void reloadConfig();

    /**
     * @brief Save current configuration
     */
    void saveConfig();

    /**
     * @brief Add a new macro
     */
    void addMacro(const QVariantMap &macro);

    /**
     * @brief Update an existing macro
     */
    void updateMacro(const QString &id, const QVariantMap &macro);

    /**
     * @brief Remove a macro
     */
    void removeMacro(const QString &id);

    /**
     * @brief Set grid layout
     */
    void setGridLayout(int columns, int rows);

signals:
    void connectedChanged();
    void discoverableChanged();
    void statusChanged();
    void deviceNameChanged();
    void macrosChanged();
    void columnsChanged();
    void rowsChanged();
    void error(const QString &message);
    void macroExecuted(const QString &macroId);

private slots:
    void onBluetoothError(const QString &message);
    void onMacroComplete();
    void onConfigError(const QString &message);

private:
    BluetoothHID *m_bluetooth;
    MacroConfig *m_config;
    QString m_currentMacroId;
};

#endif // MACROCONTROLLER_H
