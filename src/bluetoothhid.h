#ifndef BLUETOOTHHID_H
#define BLUETOOTHHID_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QString>
#include <QVariantList>
#include <QDBusConnection>
#include <QDBusInterface>

/**
 * @brief BluetoothHID - Bluetooth HID Keyboard Emulator
 * 
 * This class implements a Bluetooth HID (Human Interface Device) profile
 * that allows the Raspberry Pi Zero to act as a Bluetooth keyboard.
 * It uses the BlueZ D-Bus API and the Bluetooth HID Profile (HIDP).
 */
class BluetoothHID : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(bool discoverable READ isDiscoverable WRITE setDiscoverable NOTIFY discoverableChanged)
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

public:
    explicit BluetoothHID(QObject *parent = nullptr);
    ~BluetoothHID();

    bool isConnected() const;
    bool isDiscoverable() const;
    QString deviceName() const;
    QString status() const;

    void setDiscoverable(bool discoverable);
    void setDeviceName(const QString &name);

    // HID Keyboard scancodes
    enum class KeyCode : uint8_t {
        KEY_NONE = 0x00,
        KEY_A = 0x04,
        KEY_B = 0x05,
        KEY_C = 0x06,
        KEY_D = 0x07,
        KEY_E = 0x08,
        KEY_F = 0x09,
        KEY_G = 0x0A,
        KEY_H = 0x0B,
        KEY_I = 0x0C,
        KEY_J = 0x0D,
        KEY_K = 0x0E,
        KEY_L = 0x0F,
        KEY_M = 0x10,
        KEY_N = 0x11,
        KEY_O = 0x12,
        KEY_P = 0x13,
        KEY_Q = 0x14,
        KEY_R = 0x15,
        KEY_S = 0x16,
        KEY_T = 0x17,
        KEY_U = 0x18,
        KEY_V = 0x19,
        KEY_W = 0x1A,
        KEY_X = 0x1B,
        KEY_Y = 0x1C,
        KEY_Z = 0x1D,
        KEY_1 = 0x1E,
        KEY_2 = 0x1F,
        KEY_3 = 0x20,
        KEY_4 = 0x21,
        KEY_5 = 0x22,
        KEY_6 = 0x23,
        KEY_7 = 0x24,
        KEY_8 = 0x25,
        KEY_9 = 0x26,
        KEY_0 = 0x27,
        KEY_ENTER = 0x28,
        KEY_ESCAPE = 0x29,
        KEY_BACKSPACE = 0x2A,
        KEY_TAB = 0x2B,
        KEY_SPACE = 0x2C,
        KEY_F1 = 0x3A,
        KEY_F2 = 0x3B,
        KEY_F3 = 0x3C,
        KEY_F4 = 0x3D,
        KEY_F5 = 0x3E,
        KEY_F6 = 0x3F,
        KEY_F7 = 0x40,
        KEY_F8 = 0x41,
        KEY_F9 = 0x42,
        KEY_F10 = 0x43,
        KEY_F11 = 0x44,
        KEY_F12 = 0x45,
        KEY_PRINT_SCREEN = 0x46,
        KEY_DELETE = 0x4C,
        KEY_RIGHT_ARROW = 0x4F,
        KEY_LEFT_ARROW = 0x50,
        KEY_DOWN_ARROW = 0x51,
        KEY_UP_ARROW = 0x52,
        KEY_VOLUME_MUTE = 0x7F,
        KEY_VOLUME_UP = 0x80,
        KEY_VOLUME_DOWN = 0x81,
    };

    // Modifier keys
    enum class Modifier : uint8_t {
        NONE = 0x00,
        LEFT_CTRL = 0x01,
        LEFT_SHIFT = 0x02,
        LEFT_ALT = 0x04,
        LEFT_GUI = 0x08,  // Windows/Super key
        RIGHT_CTRL = 0x10,
        RIGHT_SHIFT = 0x20,
        RIGHT_ALT = 0x40,
        RIGHT_GUI = 0x80
    };

public slots:
    /**
     * @brief Initialize the Bluetooth HID service
     */
    bool initialize();

    /**
     * @brief Send a single key press and release
     */
    void sendKey(uint8_t keyCode, uint8_t modifiers = 0);

    /**
     * @brief Send a key combination (e.g., Ctrl+C)
     */
    void sendKeyCombo(const QVariantList &keyCodes, uint8_t modifiers);

    /**
     * @brief Send a string of text as keyboard input
     */
    void sendText(const QString &text);

    /**
     * @brief Execute a macro sequence with delays
     */
    void executeMacro(const QVariantList &sequence);

    /**
     * @brief Start pairing mode
     */
    void startPairing();

    /**
     * @brief Disconnect from current device
     */
    void disconnect();

signals:
    void connectedChanged();
    void discoverableChanged();
    void deviceNameChanged();
    void statusChanged();
    void error(const QString &message);
    void pairingRequested(const QString &deviceAddress);
    void macroComplete();

private slots:
    void onConnectionStateChanged();
    void processNextMacroStep();

private:
    void setupDBus();
    void registerHIDProfile();
    void sendHIDReport(uint8_t modifiers, uint8_t keyCode);
    void releaseAllKeys();
    uint8_t charToKeyCode(QChar c, bool &needsShift) const;

    bool m_connected;
    bool m_discoverable;
    QString m_deviceName;
    QString m_status;
    
    int m_hidInterruptFd;
    int m_hidControlFd;
    
    QVariantList m_macroQueue;
    int m_macroIndex;
    QTimer *m_macroTimer;
    
    QDBusInterface *m_bluetoothAdapter;
    QDBusInterface *m_profileManager;
};

#endif // BLUETOOTHHID_H
