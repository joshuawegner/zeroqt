cmake_minimum_required(VERSION 3.18)

project(piweather VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2 Network)

# Source files
set(PROJECT_SOURCES
    src/main.cpp
)

# Create executable
qt_add_executable(piweather
    ${PROJECT_SOURCES}
)

# Link Qt libraries
target_link_libraries(piweather PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
)

# Add QML files
qt_add_qml_module(piweather
    URI PiWeather
    VERSION 1.0
    QML_FILES
        qml/main.qml
)

# Set executable properties
set_target_properties(piweather PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Install target
install(TARGETS piweather
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
