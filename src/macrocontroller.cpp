#include "macrocontroller.h"

#include <QDebug>

MacroController::MacroController(QObject *parent)
    : QObject(parent)
    , m_bluetooth(new BluetoothHID(this))
    , m_config(new MacroConfig(this))
{
    // Connect Bluetooth signals
    connect(m_bluetooth, &BluetoothHID::connectedChanged,
            this, &MacroController::connectedChanged);
    connect(m_bluetooth, &BluetoothHID::discoverableChanged,
            this, &MacroController::discoverableChanged);
    connect(m_bluetooth, &BluetoothHID::statusChanged,
            this, &MacroController::statusChanged);
    connect(m_bluetooth, &BluetoothHID::deviceNameChanged,
            this, &MacroController::deviceNameChanged);
    connect(m_bluetooth, &BluetoothHID::error,
            this, &MacroController::onBluetoothError);
    connect(m_bluetooth, &BluetoothHID::macroComplete,
            this, &MacroController::onMacroComplete);
    
    // Connect config signals
    connect(m_config, &MacroConfig::macrosChanged,
            this, &MacroController::macrosChanged);
    connect(m_config, &MacroConfig::columnsChanged,
            this, &MacroController::columnsChanged);
    connect(m_config, &MacroConfig::rowsChanged,
            this, &MacroController::rowsChanged);
    connect(m_config, &MacroConfig::error,
            this, &MacroController::onConfigError);
}

MacroController::~MacroController()
{
}

bool MacroController::isConnected() const
{
    return m_bluetooth->isConnected();
}

bool MacroController::isDiscoverable() const
{
    return m_bluetooth->isDiscoverable();
}

QString MacroController::status() const
{
    return m_bluetooth->status();
}

QString MacroController::deviceName() const
{
    return m_bluetooth->deviceName();
}

QVariantList MacroController::macros() const
{
    return m_config->macros();
}

int MacroController::columns() const
{
    return m_config->columns();
}

int MacroController::rows() const
{
    return m_config->rows();
}

void MacroController::setDiscoverable(bool discoverable)
{
    m_bluetooth->setDiscoverable(discoverable);
}

void MacroController::setDeviceName(const QString &name)
{
    m_bluetooth->setDeviceName(name);
}

bool MacroController::initialize()
{
    qDebug() << "Initializing MacroController...";
    
    // Load macro configuration
    if (!m_config->loadConfig()) {
        qWarning() << "Failed to load macro configuration, using defaults";
    }
    
    // Initialize Bluetooth HID
    if (!m_bluetooth->initialize()) {
        qWarning() << "Failed to initialize Bluetooth HID";
        return false;
    }
    
    return true;
}

void MacroController::executeMacro(const QString &macroId)
{
    qDebug() << "Executing macro:" << macroId;
    
    if (!m_bluetooth->isConnected()) {
        emit error("Not connected to any device. Please pair first.");
        return;
    }
    
    QVariantList sequence = m_config->getMacroSequence(macroId);
    
    if (sequence.isEmpty()) {
        emit error("Macro not found: " + macroId);
        return;
    }
    
    m_currentMacroId = macroId;
    m_bluetooth->executeMacro(sequence);
}

void MacroController::startPairing()
{
    m_bluetooth->startPairing();
}

void MacroController::disconnect()
{
    m_bluetooth->disconnect();
}

void MacroController::reloadConfig()
{
    m_config->loadConfig();
}

void MacroController::saveConfig()
{
    m_config->saveConfig();
}

void MacroController::addMacro(const QVariantMap &macro)
{
    m_config->addMacro(macro);
    m_config->saveConfig();
}

void MacroController::updateMacro(const QString &id, const QVariantMap &macro)
{
    m_config->updateMacro(id, macro);
    m_config->saveConfig();
}

void MacroController::removeMacro(const QString &id)
{
    m_config->removeMacro(id);
    m_config->saveConfig();
}

void MacroController::setGridLayout(int columns, int rows)
{
    m_config->setColumns(columns);
    m_config->setRows(rows);
    m_config->saveConfig();
}

void MacroController::onBluetoothError(const QString &message)
{
    qWarning() << "Bluetooth error:" << message;
    emit error(message);
}

void MacroController::onMacroComplete()
{
    qDebug() << "Macro completed:" << m_currentMacroId;
    emit macroExecuted(m_currentMacroId);
    m_currentMacroId.clear();
}

void MacroController::onConfigError(const QString &message)
{
    qWarning() << "Config error:" << message;
    emit error(message);
}
