cmake_minimum_required(VERSION 3.18)

project(macropad VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Cross-compilation settings for Raspberry Pi Zero
if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross-compiling for Raspberry Pi Zero")
endif()

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2 DBus)

# Find BlueZ for Bluetooth HID
find_package(PkgConfig REQUIRED)
pkg_check_modules(BLUEZ REQUIRED bluez)

# Source files
set(PROJECT_SOURCES
    src/main.cpp
    src/bluetoothhid.cpp
    src/bluetoothhid.h
    src/macrocontroller.cpp
    src/macrocontroller.h
    src/macroconfig.cpp
    src/macroconfig.h
)

# Create executable
qt_add_executable(macropad
    ${PROJECT_SOURCES}
)

# Include directories
target_include_directories(macropad PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${BLUEZ_INCLUDE_DIRS}
)

# Link Qt libraries
target_link_libraries(macropad PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    ${BLUEZ_LIBRARIES}
)

# Add QML files
qt_add_qml_module(macropad
    URI MacroPad
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/MacroButton.qml
        qml/MacroGrid.qml
        qml/SettingsPage.qml
    RESOURCES
        resources/macros.json
)

# Set executable properties
set_target_properties(macropad PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Install target
install(TARGETS macropad
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install systemd service file
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/macropad.service
    DESTINATION /etc/systemd/system
)

# Install Bluetooth HID profile
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bluetooth-hid.service
    DESTINATION /etc/systemd/system
)
