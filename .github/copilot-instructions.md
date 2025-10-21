# GitHub Copilot Instructions for ZeroQt/PiWeather

## Project Overview

This is **PiWeather** (also known as embeddedqt/zeroqt) — a lightweight yet visually beautiful weather dashboard built with **C++** and **Qt 6 (QML)** for the **Raspberry Pi Zero 2 W** with an 800×480 HDMI touch display.

The application runs **directly on the framebuffer** (no X11, no Wayland, no desktop) using Qt's **EGLFS** backend — ideal for kiosk-style embedded setups.

## Key Features

- Full-screen, touch-friendly weather interface
- Animated backgrounds reacting to time and weather
- Smooth transitions and parallax cloud layers
- First-run onboarding with worldwide **address autocomplete**
- Cached offline mode for poor connectivity
- Simple, themeable QML architecture
- Designed for constrained hardware (Pi Zero 2 W)

## Technology Stack

- **Language**: C++ (C++17)
- **UI Framework**: Qt 6 (QML with QuickControls2) - Note: Currently using Qt Widgets, migrating to QML
- **Build System**: CMake (minimum version 3.18)
- **Target Platform**: Raspberry Pi Zero 2 W (ARM Cortex-A53, 512 MB RAM)
- **Display Backend**: EGLFS (framebuffer, no window manager)
- **Graphics**: OpenGL ES 2.0
- **Network**: Qt Network module for weather & geocoding APIs

## Project Structure

### Current Structure
```
zeroqt/
├── CMakeLists.txt          # CMake build configuration
├── main.cpp                # Application entry point
├── mainwindow.cpp/h        # Main window implementation
├── mainwindow.ui           # Qt Designer UI file
├── README.md               # Project documentation
└── .gitignore
```

### Planned Structure (from README)
```
piweather/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── settingsmanager.cpp/h
│   ├── geocodingmanager.cpp/h
│   ├── weatherprovider.cpp/h
│   └── …
├── qml/
│   ├── main.qml
│   ├── Onboarding.qml
│   ├── MainPage.qml
│   ├── Background.qml
│   └── SettingsPage.qml
├── resources/
│   ├── icons/
│   └── shaders/
└── build/
```

## Build System

### Build Requirements

On your **host PC** (cross-compile) **or directly on the Pi Zero 2 W**:

- **Qt 6.5+** (modules: `Core`, `Quick`, `QuickControls2`, `Network`)
- **CMake ≥ 3.18**
- **g++ 10+**
- **OpenGL ES 2.0** libraries (included with Raspberry Pi OS)
- Internet connection (for weather & geocoding APIs)

### Installing Dependencies (on the Pi)

```bash
sudo apt update
sudo apt install -y qt6-base-dev qt6-declarative-dev \
                   qt6-quickcontrols2-dev cmake g++ git
```

### Building the Project

```bash
git clone https://github.com/<youruser>/piweather.git
cd piweather
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
```

This produces the binary: `build/piweather` (currently `build/embeddedqt`)

### Key CMake Features

- **Auto-generated files**: Uses `CMAKE_AUTOUIC`, `CMAKE_AUTOMOC`, and `CMAKE_AUTORCC`
- **ARM optimization**: Reduced optimization flags (`-O1 -g0`) for ARM compilation to reduce memory usage
- **Qt version support**: Compatible with both Qt5 and Qt6
- **Output binary**: `build/embeddedqt` (will be `build/piweather`)

## Code Conventions

### C++ Style

- **Standard**: C++17
- **Naming**:
  - Classes: PascalCase (e.g., `MainWindow`)
  - Methods: camelCase (e.g., `setupUi`)
  - Private members: Use `ui` prefix for UI pointers
- **Headers**: Use header guards (`#ifndef`, `#define`, `#endif`)
- **Qt Conventions**: Follow Qt's naming patterns and use Qt macros (`Q_OBJECT`, etc.)

### File Organization

- Header files (`.h`) declare class interfaces
- Implementation files (`.cpp`) contain class definitions
- UI files (`.ui`) are Qt Designer XML files (auto-processed by CMake)

## Dependencies

### Current Qt Modules

- Qt6::Widgets (or Qt5::Widgets)
- Core, GUI (implicitly included with Widgets)

### Planned Qt Modules (from README)

- Qt6::Core
- Qt6::Quick
- Qt6::QuickControls2
- Qt6::Network (for weather & geocoding APIs)

## Platform-Specific Considerations

### Raspberry Pi Zero 2 W

- **Limited resources**: 512 MB RAM, quad-core ARM Cortex-A53
- **Optimization**: CMake automatically applies `-O1 -g0` for ARM builds to reduce memory usage
- **Display**: 800×480 touch screen via HDMI
- **Backend**: EGLFS (no X server, runs directly on framebuffer)

### Runtime Environment

The Pi Zero 2 W boots directly into the app with EGLFS, using the hardware framebuffer.

1. **Disable desktop/X server:**
```bash
sudo systemctl disable lightdm
sudo systemctl stop lightdm
```

2. **Set environment for Qt EGLFS:**
```bash
export QT_QPA_PLATFORM=eglfs
export QT_QPA_EGLFS_WIDTH=800
export QT_QPA_EGLFS_HEIGHT=480
export QT_QPA_EGLFS_HIDECURSOR=1
export QT_QPA_EGLFS_PHYSICAL_WIDTH=154  # adjust for your screen
export QT_QPA_EGLFS_PHYSICAL_HEIGHT=86
```

3. **Run the app:**
```bash
./build/piweather
```

### Auto-launch at Boot

Add to `/etc/rc.local` before `exit 0`:
```bash
su - pi -c 'cd /home/pi/piweather && QT_QPA_PLATFORM=eglfs ./build/piweather &'
```

## Application Features

### First Run (Address Entry)

On first launch:
- The app shows a touch keyboard and address field
- Autocomplete suggestions are powered by **OpenStreetMap Nominatim**
- After picking a location, it's stored locally (`~/.local/share/piweather/settings.json`)
- Subsequent launches skip onboarding and display weather directly

### Weather Data

Default provider: **OpenWeatherMap**

To use your own API key, edit `~/.local/share/piweather/settings.json` or set:
```bash
export OPENWEATHER_API_KEY="your_key_here"
```

### Privacy

- All location data is stored locally only
- Only anonymized coordinate requests are sent to geocoding and weather APIs

## Testing

Currently, the project does not have a formal test suite. When adding tests:
- Follow Qt Test framework conventions if implementing unit tests
- Consider the limited resources of the target platform
- Test on actual Raspberry Pi hardware when possible

### Expected Startup Log

```
[INFO] PiWeather starting (EGLFS)
[INFO] Screen: 800x480
[INFO] Settings loaded (address=Paris, FR)
[INFO] WeatherProvider: Using OpenWeatherMap
[INFO] UI: MainPage.qml loaded
```

## Common Tasks

### Adding New Source Files

1. Create `.h` and `.cpp` files
2. Add them to `PROJECT_SOURCES` in `CMakeLists.txt`
3. Rebuild with CMake

### Adding Qt UI Components

1. Edit `.ui` files with Qt Designer or manually
2. CMake's `AUTOUIC` will automatically generate `ui_*.h` files
3. Include generated headers as `#include "./ui_mainwindow.h"`

### Modifying Dependencies

- Update `find_package()` calls in `CMakeLists.txt`
- Add new modules to `target_link_libraries()`

## Performance Guidelines

For embedded systems like Raspberry Pi Zero 2 W:
- Runs fastest with the **EGLFS backend** (no compositor)
- Prefer **SVG icons and compressed assets**
- Use `performanceMode=true` in settings to disable particles
- Target frame rate: **~25-30 FPS**
- Prefer stack allocation over heap when possible
- Minimize dynamic allocations in UI update loops
- Use Qt's signals/slots efficiently
- Consider memory constraints (512 MB total RAM)

## Documentation

- Keep `README.md` updated with user-facing changes
- Document public APIs in header files
- Include comments for complex algorithms or platform-specific code

## Current Development Status

### Already Implemented (per README)
- Full-screen touch-friendly interface
- Animated backgrounds reacting to time and weather
- Smooth transitions and parallax cloud layers
- First-run onboarding with address autocomplete (OpenStreetMap Nominatim)
- Cached offline mode for poor connectivity
- Simple, themeable QML architecture

### Work in Progress
- Migration from Qt Widgets to QML-based UI
- Weather data integration (OpenWeatherMap)
- Settings management system
- Geocoding manager
- Weather provider abstraction

### Development Principles

When implementing features:
- Maintain the lightweight, embedded-friendly architecture
- Prioritize performance for constrained hardware (Pi Zero 2 W)
- Keep UI touch-friendly for 800×480 display
- Ensure offline functionality where possible
- Follow Qt Quick/QML best practices for the planned migration
- Design with the EGLFS framebuffer environment in mind
