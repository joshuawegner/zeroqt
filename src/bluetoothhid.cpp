#include "bluetoothhid.h"

#include <QDebug>
#include <QFile>
#include <QDBusReply>
#include <QDBusArgument>
#include <QThread>

#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

// HID Profile UUID
static const QString HID_PROFILE_UUID = "00001124-0000-1000-8000-00805f9b34fb";

// L2CAP PSM for HID
static const int L2CAP_PSM_HIDP_CTRL = 0x11;
static const int L2CAP_PSM_HIDP_INTR = 0x13;

// HID Report Descriptor for a standard keyboard
static const uint8_t HID_REPORT_DESCRIPTOR[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop)
    0x09, 0x06,  // Usage (Keyboard)
    0xA1, 0x01,  // Collection (Application)
    0x85, 0x01,  //   Report ID (1)
    0x05, 0x07,  //   Usage Page (Key Codes)
    0x19, 0xE0,  //   Usage Minimum (224)
    0x29, 0xE7,  //   Usage Maximum (231)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x01,  //   Logical Maximum (1)
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x08,  //   Report Count (8)
    0x81, 0x02,  //   Input (Data, Variable, Absolute) - Modifier byte
    0x95, 0x01,  //   Report Count (1)
    0x75, 0x08,  //   Report Size (8)
    0x81, 0x01,  //   Input (Constant) - Reserved byte
    0x95, 0x05,  //   Report Count (5)
    0x75, 0x01,  //   Report Size (1)
    0x05, 0x08,  //   Usage Page (LEDs)
    0x19, 0x01,  //   Usage Minimum (1)
    0x29, 0x05,  //   Usage Maximum (5)
    0x91, 0x02,  //   Output (Data, Variable, Absolute) - LED report
    0x95, 0x01,  //   Report Count (1)
    0x75, 0x03,  //   Report Size (3)
    0x91, 0x01,  //   Output (Constant) - LED report padding
    0x95, 0x06,  //   Report Count (6)
    0x75, 0x08,  //   Report Size (8)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x65,  //   Logical Maximum (101)
    0x05, 0x07,  //   Usage Page (Key Codes)
    0x19, 0x00,  //   Usage Minimum (0)
    0x29, 0x65,  //   Usage Maximum (101)
    0x81, 0x00,  //   Input (Data, Array) - Key arrays (6 keys)
    0xC0         // End Collection
};

BluetoothHID::BluetoothHID(QObject *parent)
    : QObject(parent)
    , m_connected(false)
    , m_discoverable(false)
    , m_deviceName("MacroPad")
    , m_status("Not initialized")
    , m_hidInterruptFd(-1)
    , m_hidControlFd(-1)
    , m_macroIndex(0)
    , m_macroTimer(new QTimer(this))
    , m_bluetoothAdapter(nullptr)
    , m_profileManager(nullptr)
{
    connect(m_macroTimer, &QTimer::timeout, this, &BluetoothHID::processNextMacroStep);
}

BluetoothHID::~BluetoothHID()
{
    disconnect();
    
    if (m_hidInterruptFd >= 0) {
        ::close(m_hidInterruptFd);
    }
    if (m_hidControlFd >= 0) {
        ::close(m_hidControlFd);
    }
}

bool BluetoothHID::isConnected() const
{
    return m_connected;
}

bool BluetoothHID::isDiscoverable() const
{
    return m_discoverable;
}

QString BluetoothHID::deviceName() const
{
    return m_deviceName;
}

QString BluetoothHID::status() const
{
    return m_status;
}

void BluetoothHID::setDiscoverable(bool discoverable)
{
    if (m_discoverable != discoverable) {
        m_discoverable = discoverable;
        
        if (m_bluetoothAdapter) {
            m_bluetoothAdapter->setProperty("Discoverable", discoverable);
            m_bluetoothAdapter->setProperty("Pairable", discoverable);
        }
        
        emit discoverableChanged();
    }
}

void BluetoothHID::setDeviceName(const QString &name)
{
    if (m_deviceName != name) {
        m_deviceName = name;
        
        if (m_bluetoothAdapter) {
            m_bluetoothAdapter->setProperty("Alias", name);
        }
        
        emit deviceNameChanged();
    }
}

bool BluetoothHID::initialize()
{
    m_status = "Initializing Bluetooth HID...";
    emit statusChanged();
    
    setupDBus();
    
    if (!m_bluetoothAdapter || !m_bluetoothAdapter->isValid()) {
        m_status = "Error: No Bluetooth adapter found";
        emit statusChanged();
        emit error("No Bluetooth adapter found");
        return false;
    }
    
    // Power on the adapter
    m_bluetoothAdapter->setProperty("Powered", true);
    
    // Set the device name
    m_bluetoothAdapter->setProperty("Alias", m_deviceName);
    
    // Register the HID profile
    registerHIDProfile();
    
    m_status = "Ready - Waiting for connection";
    emit statusChanged();
    
    return true;
}

void BluetoothHID::setupDBus()
{
    QDBusConnection systemBus = QDBusConnection::systemBus();
    
    if (!systemBus.isConnected()) {
        qWarning() << "Cannot connect to D-Bus system bus";
        return;
    }
    
    // Get the Bluetooth adapter
    m_bluetoothAdapter = new QDBusInterface(
        "org.bluez",
        "/org/bluez/hci0",
        "org.bluez.Adapter1",
        systemBus,
        this
    );
    
    // Get the profile manager
    m_profileManager = new QDBusInterface(
        "org.bluez",
        "/org/bluez",
        "org.bluez.ProfileManager1",
        systemBus,
        this
    );
}

void BluetoothHID::registerHIDProfile()
{
    if (!m_profileManager || !m_profileManager->isValid()) {
        qWarning() << "Profile manager not available";
        return;
    }
    
    // Create the HID profile options
    QVariantMap options;
    options["Name"] = "MacroPad Keyboard";
    options["Role"] = "server";
    options["RequireAuthentication"] = false;
    options["RequireAuthorization"] = false;
    options["ServiceRecord"] = QString::fromLatin1(
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
        "<record>"
        "<attribute id=\"0x0001\">"
        "<sequence><uuid value=\"0x1124\" /></sequence>"
        "</attribute>"
        "<attribute id=\"0x0004\">"
        "<sequence>"
        "<sequence><uuid value=\"0x0100\" /><uint16 value=\"0x0011\" /></sequence>"
        "<sequence><uuid value=\"0x0011\" /></sequence>"
        "</sequence>"
        "</attribute>"
        "<attribute id=\"0x0005\">"
        "<sequence><uuid value=\"0x1002\" /></sequence>"
        "</attribute>"
        "<attribute id=\"0x0006\">"
        "<sequence>"
        "<uint16 value=\"0x656e\" /><uint16 value=\"0x006a\" /><uint16 value=\"0x0100\" />"
        "</sequence>"
        "</attribute>"
        "<attribute id=\"0x0009\">"
        "<sequence>"
        "<sequence><uuid value=\"0x1124\" /><uint16 value=\"0x0100\" /></sequence>"
        "</sequence>"
        "</attribute>"
        "<attribute id=\"0x000d\">"
        "<sequence>"
        "<sequence><uuid value=\"0x0100\" /><uint16 value=\"0x0013\" /></sequence>"
        "<sequence><uuid value=\"0x0011\" /></sequence>"
        "</sequence>"
        "</attribute>"
        "<attribute id=\"0x0100\"><text value=\"MacroPad Keyboard\" /></attribute>"
        "<attribute id=\"0x0101\"><text value=\"Raspberry Pi MacroPad\" /></attribute>"
        "<attribute id=\"0x0102\"><text value=\"MacroPad\" /></attribute>"
        "<attribute id=\"0x0200\"><uint16 value=\"0x0100\" /></attribute>"
        "<attribute id=\"0x0201\"><uint16 value=\"0x0111\" /></attribute>"
        "<attribute id=\"0x0202\"><uint8 value=\"0x40\" /></attribute>"
        "<attribute id=\"0x0203\"><uint8 value=\"0x00\" /></attribute>"
        "<attribute id=\"0x0204\"><boolean value=\"true\" /></attribute>"
        "<attribute id=\"0x0205\"><boolean value=\"true\" /></attribute>"
        "<attribute id=\"0x0206\">"
        "<sequence><sequence>"
        "<uint8 value=\"0x22\" />"
        "<text encoding=\"hex\" value=\""
        "05010906a101850105071900e029e7150025017501950881029501750881010595"
        "0175010508190129059102950175039101950675081500652605071900296581000"
        "c0\" />"
        "</sequence></sequence>"
        "</attribute>"
        "</record>"
    );
    
    QDBusReply<void> reply = m_profileManager->call(
        "RegisterProfile",
        QDBusObjectPath("/org/bluez/macropad"),
        HID_PROFILE_UUID,
        options
    );
    
    if (!reply.isValid()) {
        qWarning() << "Failed to register HID profile:" << reply.error().message();
    }
}

void BluetoothHID::sendHIDReport(uint8_t modifiers, uint8_t keyCode)
{
    if (m_hidInterruptFd < 0) {
        qWarning() << "HID interrupt channel not connected";
        return;
    }
    
    // HID keyboard report format:
    // Byte 0: Report ID (0xA1 for INPUT report)
    // Byte 1: Modifier keys
    // Byte 2: Reserved (0x00)
    // Bytes 3-8: Key codes (up to 6 simultaneous keys)
    uint8_t report[10] = {
        0xA1,       // INPUT report
        0x01,       // Report ID
        modifiers,  // Modifier keys
        0x00,       // Reserved
        keyCode,    // Key code
        0x00,       // Additional keys (unused)
        0x00,
        0x00,
        0x00,
        0x00
    };
    
    ssize_t written = write(m_hidInterruptFd, report, sizeof(report));
    if (written < 0) {
        qWarning() << "Failed to send HID report";
        emit error("Failed to send key press");
    }
}

void BluetoothHID::releaseAllKeys()
{
    sendHIDReport(0x00, 0x00);
}

void BluetoothHID::sendKey(uint8_t keyCode, uint8_t modifiers)
{
    if (!m_connected) {
        emit error("Not connected to any device");
        return;
    }
    
    // Send key press
    sendHIDReport(modifiers, keyCode);
    
    // Small delay
    QThread::msleep(10);
    
    // Send key release
    releaseAllKeys();
}

void BluetoothHID::sendKeyCombo(const QVariantList &keyCodes, uint8_t modifiers)
{
    if (!m_connected) {
        emit error("Not connected to any device");
        return;
    }
    
    // For key combinations, we need to send all keys pressed together
    // then release them all
    for (const QVariant &keyVar : keyCodes) {
        uint8_t keyCode = static_cast<uint8_t>(keyVar.toUInt());
        sendHIDReport(modifiers, keyCode);
        QThread::msleep(10);
    }
    
    releaseAllKeys();
}

uint8_t BluetoothHID::charToKeyCode(QChar c, bool &needsShift) const
{
    needsShift = false;
    
    if (c >= 'a' && c <= 'z') {
        return static_cast<uint8_t>(KeyCode::KEY_A) + (c.unicode() - 'a');
    }
    
    if (c >= 'A' && c <= 'Z') {
        needsShift = true;
        return static_cast<uint8_t>(KeyCode::KEY_A) + (c.unicode() - 'A');
    }
    
    if (c >= '1' && c <= '9') {
        return static_cast<uint8_t>(KeyCode::KEY_1) + (c.unicode() - '1');
    }
    
    if (c == '0') {
        return static_cast<uint8_t>(KeyCode::KEY_0);
    }
    
    if (c == ' ') {
        return static_cast<uint8_t>(KeyCode::KEY_SPACE);
    }
    
    if (c == '\n' || c == '\r') {
        return static_cast<uint8_t>(KeyCode::KEY_ENTER);
    }
    
    if (c == '\t') {
        return static_cast<uint8_t>(KeyCode::KEY_TAB);
    }
    
    // Handle special characters (simplified)
    switch (c.unicode()) {
        case '!': needsShift = true; return static_cast<uint8_t>(KeyCode::KEY_1);
        case '@': needsShift = true; return static_cast<uint8_t>(KeyCode::KEY_2);
        case '#': needsShift = true; return static_cast<uint8_t>(KeyCode::KEY_3);
        case '$': needsShift = true; return static_cast<uint8_t>(KeyCode::KEY_4);
        case '%': needsShift = true; return static_cast<uint8_t>(KeyCode::KEY_5);
        case '^': needsShift = true; return static_cast<uint8_t>(KeyCode::KEY_6);
        case '&': needsShift = true; return static_cast<uint8_t>(KeyCode::KEY_7);
        case '*': needsShift = true; return static_cast<uint8_t>(KeyCode::KEY_8);
        case '(': needsShift = true; return static_cast<uint8_t>(KeyCode::KEY_9);
        case ')': needsShift = true; return static_cast<uint8_t>(KeyCode::KEY_0);
        default: return 0x00;
    }
}

void BluetoothHID::sendText(const QString &text)
{
    if (!m_connected) {
        emit error("Not connected to any device");
        return;
    }
    
    for (const QChar &c : text) {
        bool needsShift = false;
        uint8_t keyCode = charToKeyCode(c, needsShift);
        
        if (keyCode != 0x00) {
            uint8_t modifiers = needsShift ? static_cast<uint8_t>(Modifier::LEFT_SHIFT) : 0;
            sendKey(keyCode, modifiers);
            QThread::msleep(20);  // Small delay between characters
        }
    }
}

void BluetoothHID::executeMacro(const QVariantList &sequence)
{
    if (!m_connected) {
        emit error("Not connected to any device");
        return;
    }
    
    m_macroQueue = sequence;
    m_macroIndex = 0;
    
    // Start processing the macro
    processNextMacroStep();
}

void BluetoothHID::processNextMacroStep()
{
    if (m_macroIndex >= m_macroQueue.size()) {
        m_macroQueue.clear();
        m_macroIndex = 0;
        emit macroComplete();
        return;
    }
    
    QVariantMap step = m_macroQueue[m_macroIndex].toMap();
    m_macroIndex++;
    
    QString type = step.value("type").toString();
    
    if (type == "key") {
        uint8_t keyCode = static_cast<uint8_t>(step.value("keyCode").toUInt());
        uint8_t modifiers = static_cast<uint8_t>(step.value("modifiers", 0).toUInt());
        sendKey(keyCode, modifiers);
    } else if (type == "text") {
        sendText(step.value("text").toString());
    } else if (type == "delay") {
        int delayMs = step.value("ms", 100).toInt();
        m_macroTimer->start(delayMs);
        return;  // Don't process next step immediately
    } else if (type == "combo") {
        QVariantList keys = step.value("keys").toList();
        uint8_t modifiers = static_cast<uint8_t>(step.value("modifiers", 0).toUInt());
        sendKeyCombo(keys, modifiers);
    }
    
    // Process next step with a small delay
    m_macroTimer->start(30);
}

void BluetoothHID::startPairing()
{
    setDiscoverable(true);
    m_status = "Pairing mode - Waiting for connection...";
    emit statusChanged();
}

void BluetoothHID::disconnect()
{
    if (m_hidInterruptFd >= 0) {
        ::close(m_hidInterruptFd);
        m_hidInterruptFd = -1;
    }
    
    if (m_hidControlFd >= 0) {
        ::close(m_hidControlFd);
        m_hidControlFd = -1;
    }
    
    if (m_connected) {
        m_connected = false;
        emit connectedChanged();
    }
    
    m_status = "Disconnected";
    emit statusChanged();
}

void BluetoothHID::onConnectionStateChanged()
{
    // This would be called by D-Bus signals when connection state changes
    emit connectedChanged();
    emit statusChanged();
}
