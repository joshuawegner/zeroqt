# 🎹 MacroPad — Bluetooth Macro Keyboard for Raspberry Pi Zero

A touchscreen macro pad application built with **Qt 6 (QML)** that turns your **Raspberry Pi Zero** into a **Bluetooth HID keyboard**. Perfect for streaming, productivity shortcuts, or custom hotkeys.

The Pi connects to your PC/Mac/Linux just like a regular Bluetooth keyboard, but instead of physical keys, you get customizable touch buttons on your display.

---

## ✨ Features

- **Bluetooth HID Keyboard** - Connects to any PC/Mac/Linux as a standard Bluetooth keyboard
- **Touchscreen Interface** - Large, customizable macro buttons with emoji icons
- **12 Default Macros** - Copy, Paste, Cut, Undo, Redo, Save, and more
- **JSON Configuration** - Easy to customize macros without recompiling
- **Material Design** - Modern dark theme optimized for touch
- **Auto-start** - Runs on boot via systemd
- **Framebuffer Rendering** - No X11/Wayland needed (EGLFS)

---

## 🖥️ Hardware Requirements

- **Raspberry Pi Zero W** or **Zero 2 W** (Bluetooth required)
- **Touchscreen Display** (tested with 3.5" - 7" HDMI/DSI displays)
- **MicroSD Card** (8GB+ recommended)
- **Power Supply** (5V 2A recommended)

---

## 📁 Project Structure

```
macropad/
├── CMakeLists.txt          # CMake build configuration
├── src/
│   ├── main.cpp            # Application entry point
│   ├── bluetoothhid.cpp/h  # Bluetooth HID implementation
│   ├── macrocontroller.cpp/h # Main controller
│   └── macroconfig.cpp/h   # Macro configuration manager
├── qml/
│   ├── main.qml            # Main window
│   ├── MacroButton.qml     # Single macro button
│   ├── MacroGrid.qml       # Button grid layout
│   └── SettingsPage.qml    # Settings interface
├── resources/
│   └── macros.json         # Default macro configuration
├── scripts/
│   ├── macropad.service    # Systemd service
│   ├── bluetooth-hid.service
│   └── setup-bluetooth.sh  # Bluetooth setup script
└── .github/
    └── workflows/
        └── build.yml       # GitHub Actions CI
```

---

## 🚀 Quick Start

### Option 1: Download Pre-built Package

1. Go to [Releases](../../releases)
2. Download `macropad-rpi-zero.tar.gz`
3. Extract and run:
   ```bash
   tar -xzvf macropad-rpi-zero.tar.gz
   cd macropad
   sudo ./install.sh
   ```

### Option 2: Build from Source

```bash
# Clone the repository
git clone https://github.com/joshuawegner/zeroqt.git
cd zeroqt

# Install dependencies
sudo apt-get update
sudo apt-get install -y \
    qt6-base-dev \
    qt6-declarative-dev \
    qt6-quickcontrols2-dev \
    cmake g++ \
    bluez libbluetooth-dev

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install
sudo make install
```

---

## ⚙️ Configuration

### Environment Variables for Display

```bash
export QT_QPA_PLATFORM=eglfs
export QT_QPA_EGLFS_WIDTH=800
export QT_QPA_EGLFS_HEIGHT=480
export QT_QPA_EGLFS_HIDECURSOR=0
```

### Custom Macros

Edit `~/.config/macropad/macros.json`:

```json
{
    "columns": 4,
    "rows": 3,
    "macros": [
        {
            "id": "custom1",
            "name": "My Macro",
            "icon": "🎮",
            "color": "#FF5722",
            "sequence": [
                {"type": "key", "keyCode": 4, "modifiers": 1},
                {"type": "delay", "ms": 100},
                {"type": "text", "text": "Hello World"}
            ]
        }
    ]
}
```

### Macro Action Types

| Type | Description | Example |
|------|-------------|---------|
| `key` | Single key press | `{"type": "key", "keyCode": 6, "modifiers": 1}` (Ctrl+C) |
| `text` | Type a string | `{"type": "text", "text": "Hello"}` |
| `delay` | Wait in ms | `{"type": "delay", "ms": 100}` |
| `combo` | Multiple keys | `{"type": "combo", "keys": [4, 5], "modifiers": 1}` |

### Key Codes Reference

| Key | Code | Key | Code | Key | Code |
|-----|------|-----|------|-----|------|
| A | 4 | N | 17 | F1 | 58 |
| B | 5 | O | 18 | F2 | 59 |
| C | 6 | P | 19 | F3 | 60 |
| D | 7 | Q | 20 | F4 | 61 |
| E | 8 | R | 21 | F5 | 62 |
| F | 9 | S | 22 | F6 | 63 |
| G | 10 | T | 23 | F7 | 64 |
| H | 11 | U | 24 | F8 | 65 |
| I | 12 | V | 25 | F9 | 66 |
| J | 13 | W | 26 | F10 | 67 |
| K | 14 | X | 27 | F11 | 68 |
| L | 15 | Y | 28 | F12 | 69 |
| M | 16 | Z | 29 | Enter | 40 |

### Modifier Keys

| Modifier | Value | Key |
|----------|-------|-----|
| None | 0 | - |
| Left Ctrl | 1 | Ctrl |
| Left Shift | 2 | Shift |
| Left Alt | 4 | Alt |
| Left GUI | 8 | Win/Cmd |
| Right Ctrl | 16 | Ctrl |
| Right Shift | 32 | Shift |
| Right Alt | 64 | Alt |
| Right GUI | 128 | Win/Cmd |

---

## 📱 Pairing with Your PC

1. **On your Pi**: Start the MacroPad app and tap "Pair"
2. **On your PC**: Open Bluetooth settings
3. **Search** for "MacroPad"
4. **Pair** - no PIN required
5. **Done!** - Your Pi is now a keyboard

### Windows Pairing
1. Go to Settings → Bluetooth & devices
2. Click "Add device" → "Bluetooth"
3. Select "MacroPad" from the list

### macOS Pairing
1. Go to System Preferences → Bluetooth
2. Click "Connect" next to MacroPad

### Linux Pairing
```bash
bluetoothctl
scan on
pair <MAC_ADDRESS>
connect <MAC_ADDRESS>
```

---

## 🛠️ Systemd Auto-start

The installation script sets up auto-start, but you can manually enable it:

```bash
# Enable auto-start
sudo systemctl enable macropad
sudo systemctl enable bluetooth-hid

# Start now
sudo systemctl start macropad

# Check status
sudo systemctl status macropad
```

---

## 🔧 Troubleshooting

### Bluetooth not working
```bash
# Check Bluetooth service
sudo systemctl status bluetooth

# Restart Bluetooth
sudo systemctl restart bluetooth

# Check adapter
hciconfig

# Enable Bluetooth adapter
sudo hciconfig hci0 up
```

### Display not showing
```bash
# List available displays
cat /sys/class/drm/*/status

# Try different platform
export QT_QPA_PLATFORM=linuxfb

# For DSI displays
export QT_QPA_PLATFORM=eglfs
export QT_QPA_EGLFS_INTEGRATION=eglfs_kms
```

### Permission errors
```bash
# Add user to Bluetooth group
sudo usermod -a -G bluetooth $USER

# Reboot
sudo reboot
```

### Macro not working
```bash
# Check macro config
cat ~/.config/macropad/macros.json

# Reset to defaults
rm ~/.config/macropad/macros.json
# Restart app
```

---

## 🔨 Development

### Building for Development
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/macropad
```

### Project Dependencies
- Qt 6.5+ (Core, Quick, QuickControls2, DBus)
- BlueZ 5.50+ (Bluetooth stack)
- CMake 3.18+
- GCC 10+ or Clang 12+

---

## 🔄 CI/CD with GitHub Actions

The project includes automatic builds via GitHub Actions:

- **Triggers**: Push to main/master, Pull Requests
- **Artifacts**: Deployable package for Raspberry Pi
- **Releases**: Automatic release creation on tags

To create a release:
```bash
git tag v1.0.0
git push origin v1.0.0
```

---

## 📄 License

MIT License - feel free to use and modify!

---

## 🙏 Credits

- Qt Framework by The Qt Company
- BlueZ Bluetooth stack
- Raspberry Pi Foundation
- Material Design icons

---

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

---

Made with ❤️ for the Raspberry Pi community
