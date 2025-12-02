# 🎹 MacroPad — Bluetooth Macro Keyboard for Raspberry Pi Zero# 🌤️ PiWeather — Qt6 Weather App for Raspberry Pi Zero 2 W



A touchscreen macro pad application built with **Qt 6 (QML)** that turns your **Raspberry Pi Zero** into a **Bluetooth HID keyboard**. Perfect for streaming, productivity shortcuts, or custom hotkeys.A lightweight yet visually beautiful weather dashboard built in **C++ / Qt 6 (QML)** for the **Raspberry Pi Zero 2 W** with an **800×480 HDMI touch display**.



The Pi connects to your PC just like a regular Bluetooth keyboard, but instead of physical keys, you get customizable touch buttons on your display.It runs **directly on the framebuffer** (no X11, no Wayland, no desktop) using Qt’s **EGLFS** backend — ideal for kiosk-style embedded setups.



------



## ✨ Features## ✨ Features



- **Bluetooth HID Keyboard** - Connects to any PC/Mac/Linux as a standard Bluetooth keyboard- Full-screen, touch-friendly weather interface  

- **Touchscreen Interface** - Large, customizable macro buttons- Animated backgrounds reacting to time and weather  

- **12 Default Macros** - Copy, Paste, Cut, Undo, Redo, Save, and more- Smooth transitions and parallax cloud layers  

- **JSON Configuration** - Easy to customize macros without recompiling- First-run onboarding with worldwide **address autocomplete**  

- **Material Design** - Modern dark theme optimized for touch- Cached offline mode for poor connectivity  

- **Auto-start** - Runs on boot via systemd- Simple, themeable QML architecture  

- **Framebuffer Rendering** - No X11/Wayland needed (EGLFS)- Designed for constrained hardware (Pi Zero 2 W)



------



## 🖥️ Hardware Requirements## 🧱 Project Structure



- **Raspberry Pi Zero W** or **Zero 2 W** (Bluetooth required)piweather/

- **Touchscreen Display** (tested with 3.5" - 7" HDMI/DSI displays)├── CMakeLists.txt

- **MicroSD Card** (8GB+)├── src/

- **Power Supply** (5V 2A recommended)│   ├── main.cpp

│   ├── settingsmanager.cpp/h

---│   ├── geocodingmanager.cpp/h

│   ├── weatherprovider.cpp/h

## 📁 Project Structure│   └── …

├── qml/

```│   ├── main.qml

macropad/│   ├── Onboarding.qml

├── CMakeLists.txt          # CMake build configuration│   ├── MainPage.qml

├── src/│   ├── Background.qml

│   ├── main.cpp            # Application entry point│   └── SettingsPage.qml

│   ├── bluetoothhid.cpp/h  # Bluetooth HID implementation├── resources/

│   ├── macrocontroller.cpp/h # Main controller│   ├── icons/

│   └── macroconfig.cpp/h   # Macro configuration manager│   └── shaders/

├── qml/└── build/

│   ├── main.qml            # Main window

│   ├── MacroButton.qml     # Single macro button---

│   ├── MacroGrid.qml       # Button grid layout

│   └── SettingsPage.qml    # Settings interface## ⚙️ Build Requirements

├── resources/

│   └── macros.json         # Default macro configurationOn your **host PC** (cross-compile) **or directly on the Pi Zero 2 W**:

├── scripts/

│   ├── macropad.service    # Systemd service- **Qt 6.5 +** (modules: `Core`, `Quick`, `QuickControls2`, `Network`)

│   ├── bluetooth-hid.service- **CMake ≥ 3.18**

│   └── setup-bluetooth.sh  # Bluetooth setup script- **g++ 10 +**

└── .github/- **OpenGL ES 2.0** libraries (included with Raspberry Pi OS)

    └── workflows/- Internet connection (for weather & geocoding APIs)

        └── build.yml       # GitHub Actions CI

```### Install Qt and build tools (on the Pi)



---```bash

sudo apt update

## 🚀 Quick Startsudo apt install -y qt6-base-dev qt6-declarative-dev \

                   qt6-quickcontrols2-dev cmake g++ git

### Option 1: Download Pre-built Package



1. Go to [Releases](../../releases)⸻

2. Download `macropad-rpi-zero.tar.gz`

3. Extract and run:🏗️ Building

   ```bash

   tar -xzvf macropad-rpi-zero.tar.gzgit clone https://github.com/<youruser>/piweather.git

   cd macropadcd piweather

   sudo ./install.shcmake -B build -DCMAKE_BUILD_TYPE=Release

   ```cmake --build build -j4



### Option 2: Build from SourceThis produces the binary:



```bashbuild/piweather

# Clone the repository

git clone https://github.com/joshuawegner/zeroqt.git

cd zeroqt⸻



# Install dependencies🧭 Running on the Framebuffer (No Window Manager)

sudo apt-get update

sudo apt-get install -y \The Pi Zero 2 W will boot directly into the app with EGLFS, using the hardware framebuffer.

    qt6-base-dev \	1.	Ensure no desktop or X server is running:

    qt6-declarative-dev \

    qt6-quickcontrols2-dev \sudo systemctl disable lightdm

    cmake g++ \sudo systemctl stop lightdm

    bluez libbluetooth-dev



# Build	2.	Set environment for Qt EGLFS:

mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Releaseexport QT_QPA_PLATFORM=eglfs

make -j$(nproc)export QT_QPA_EGLFS_WIDTH=800

export QT_QPA_EGLFS_HEIGHT=480

# Installexport QT_QPA_EGLFS_HIDECURSOR=1

sudo make installexport QT_QPA_EGLFS_PHYSICAL_WIDTH=154  # adjust for your screen

```export QT_QPA_EGLFS_PHYSICAL_HEIGHT=86



---

	3.	Run the app:

## ⚙️ Configuration

./build/piweather

### Environment Variables for Display



```bash

export QT_QPA_PLATFORM=eglfsIt will start full-screen on the HDMI touch display and capture all input.

export QT_QPA_EGLFS_WIDTH=800

export QT_QPA_EGLFS_HEIGHT=480To auto-launch at boot, add this to /etc/rc.local before exit 0:

export QT_QPA_EGLFS_HIDECURSOR=0

```su - pi -c 'cd /home/pi/piweather && QT_QPA_PLATFORM=eglfs ./build/piweather &'



### Custom Macros

⸻

Edit `~/.config/macropad/macros.json`:

🌍 First Run (Address Entry)

```json

{On first launch:

    "columns": 4,	•	The app shows a touch keyboard and address field.

    "rows": 3,	•	Autocomplete suggestions are powered by OpenStreetMap Nominatim.

    "macros": [	•	After you pick a location, it’s stored locally (~/.local/share/piweather/settings.json).

        {

            "id": "custom1",Subsequent launches skip onboarding and display the weather directly.

            "name": "My Macro",

            "icon": "🎮",⸻

            "color": "#FF5722",

            "sequence": [☁️ Weather Data

                {"type": "key", "keyCode": 4, "modifiers": 1},

                {"type": "delay", "ms": 100},Default provider: OpenWeatherMap

                {"type": "text", "text": "Hello World"}

            ]To use your own API key, edit ~/.local/share/piweather/settings.json or set the environment variable:

        }

    ]export OPENWEATHER_API_KEY="your_key_here"

}

```

⸻

### Macro Action Types

⚡ Performance Tips for Pi Zero 2 W

| Type | Description | Example |	•	Runs fastest with the EGLFS backend (no compositor).

|------|-------------|---------|	•	Prefer SVG icons and compressed assets.

| `key` | Single key press | `{"type": "key", "keyCode": 6, "modifiers": 1}` (Ctrl+C) |	•	Use performanceMode=true in settings to disable particles.

| `text` | Type a string | `{"type": "text", "text": "Hello"}` |	•	Target frame rate: ~25–30 FPS.

| `delay` | Wait in ms | `{"type": "delay", "ms": 100}` |

| `combo` | Multiple keys | `{"type": "combo", "keys": [4, 5], "modifiers": 1}` |⸻



### Key Codes Reference🔒 Privacy



| Key | Code | Key | Code | Key | Code |All location data is stored locally only.

|-----|------|-----|------|-----|------|Only anonymized coordinate requests are sent to geocoding and weather APIs.

| A | 4 | N | 17 | F1 | 58 |

| B | 5 | O | 18 | F2 | 59 |⸻

| C | 6 | P | 19 | F3 | 60 |

| D | 7 | Q | 20 | F4 | 61 |📜 Licenses

| E | 8 | R | 21 | F5 | 62 |

| F | 9 | S | 22 | F6 | 63 |Weather data courtesy of OpenWeatherMap

| G | 10 | T | 23 | F7 | 64 |Geocoding powered by OpenStreetMap Nominatim

| H | 11 | U | 24 | F8 | 65 |

| I | 12 | V | 25 | F9 | 66 |⸻

| J | 13 | W | 26 | F10 | 67 |

| K | 14 | X | 27 | F11 | 68 |Example startup log (expected)

| L | 15 | Y | 28 | F12 | 69 |

| M | 16 | Z | 29 | Enter | 40 |[INFO] PiWeather starting (EGLFS)

[INFO] Screen: 800x480

### Modifier Keys[INFO] Settings loaded (address=Paris, FR)

[INFO] WeatherProvider: Using OpenWeatherMap

| Modifier | Value | Key |[INFO] UI: MainPage.qml loaded

|----------|-------|-----|

| None | 0 | - |

| Left Ctrl | 1 | Ctrl |⸻

| Left Shift | 2 | Shift |

| Left Alt | 4 | Alt |Enjoy this touch-friendly animated weather dashboard ☀️

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
