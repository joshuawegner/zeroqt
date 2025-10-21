# GitHub Copilot Instructions for ZeroQt/PiWeather

## Project Overview

This is **PiWeather** (also known as embeddedqt/zeroqt) — a lightweight, touch-friendly weather dashboard application built with **C++** and **Qt 6** for the **Raspberry Pi Zero 2 W** with an 800×480 HDMI touch display.

The application runs directly on the framebuffer using Qt's **EGLFS** backend without X11 or Wayland, making it ideal for kiosk-style embedded setups.

## Technology Stack

- **Language**: C++ (C++17)
- **UI Framework**: Qt 6 (Qt Widgets)
- **Build System**: CMake (minimum version 3.16)
- **Target Platform**: Raspberry Pi Zero 2 W (ARM architecture)
- **Display Backend**: EGLFS (framebuffer, no window manager)
- **Graphics**: OpenGL ES 2.0

## Project Structure

```
zeroqt/
├── CMakeLists.txt          # CMake build configuration
├── main.cpp                # Application entry point
├── mainwindow.cpp/h        # Main window implementation
├── mainwindow.ui           # Qt Designer UI file
├── README.md               # Project documentation
└── .gitignore
```

## Build System

### Building the Project

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
```

### Key CMake Features

- **Auto-generated files**: Uses `CMAKE_AUTOUIC`, `CMAKE_AUTOMOC`, and `CMAKE_AUTORCC`
- **ARM optimization**: Reduced optimization flags (`-O1 -g0`) for ARM compilation to reduce memory usage
- **Qt version support**: Compatible with both Qt5 and Qt6
- **Output binary**: `build/embeddedqt`

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

### Required Qt Modules

- Qt6::Widgets (or Qt5::Widgets)
- Core, GUI (implicitly included with Widgets)

### System Requirements

- Qt 6.5+ (or Qt 5.15+)
- CMake ≥ 3.16
- g++ 10+
- OpenGL ES 2.0 libraries (Raspberry Pi OS includes these)

## Platform-Specific Considerations

### Raspberry Pi Zero 2 W

- **Limited resources**: 512 MB RAM, quad-core ARM Cortex-A53
- **Optimization**: CMake automatically applies `-O1 -g0` for ARM builds to reduce memory usage
- **Display**: 800×480 touch screen via HDMI
- **Backend**: EGLFS (no X server, runs directly on framebuffer)

### Runtime Environment

```bash
export QT_QPA_PLATFORM=eglfs
export QT_QPA_EGLFS_WIDTH=800
export QT_QPA_EGLFS_HEIGHT=480
export QT_QPA_EGLFS_HIDECURSOR=1
```

## Testing

Currently, the project does not have a formal test suite. When adding tests:
- Follow Qt Test framework conventions if implementing unit tests
- Consider the limited resources of the target platform
- Test on actual Raspberry Pi hardware when possible

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
- Prefer stack allocation over heap when possible
- Minimize dynamic allocations in UI update loops
- Use Qt's signals/slots efficiently
- Consider memory constraints (512 MB total RAM)
- Target frame rate: 25-30 FPS for animations

## Documentation

- Keep `README.md` updated with user-facing changes
- Document public APIs in header files
- Include comments for complex algorithms or platform-specific code

## Future Plans (from README)

The README mentions plans for:
- QML-based UI (currently uses Qt Widgets)
- Weather data integration (OpenWeatherMap)
- Geocoding (OpenStreetMap Nominatim)
- Settings management
- First-run onboarding with address autocomplete
- Cached offline mode

When implementing these features, maintain the lightweight, embedded-friendly architecture.
