# 🌤️ PiWeather — Qt6 Weather App for Raspberry Pi Zero 2 W

A lightweight yet visually beautiful weather dashboard built in **C++ / Qt 6 (QML)** for the **Raspberry Pi Zero 2 W** with an **800×480 HDMI touch display**.

It runs **directly on the framebuffer** (no X11, no Wayland, no desktop) using Qt’s **EGLFS** backend — ideal for kiosk-style embedded setups.

---

## ✨ Features

- Full-screen, touch-friendly weather interface  
- Animated backgrounds reacting to time and weather  
- Smooth transitions and parallax cloud layers  
- First-run onboarding with worldwide **address autocomplete**  
- Cached offline mode for poor connectivity  
- Simple, themeable QML architecture  
- Designed for constrained hardware (Pi Zero 2 W)

---

## 🧱 Project Structure

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

---

## ⚙️ Build Requirements

On your **host PC** (cross-compile) **or directly on the Pi Zero 2 W**:

- **Qt 6.5 +** (modules: `Core`, `Quick`, `QuickControls2`, `Network`)
- **CMake ≥ 3.18**
- **g++ 10 +**
- **OpenGL ES 2.0** libraries (included with Raspberry Pi OS)
- Internet connection (for weather & geocoding APIs)

### Install Qt and build tools (on the Pi)

```bash
sudo apt update
sudo apt install -y qt6-base-dev qt6-declarative-dev \
                   qt6-quickcontrols2-dev cmake g++ git


⸻

🏗️ Building

git clone https://github.com/<youruser>/piweather.git
cd piweather
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4

This produces the binary:

build/piweather


⸻

🧭 Running on the Framebuffer (No Window Manager)

The Pi Zero 2 W will boot directly into the app with EGLFS, using the hardware framebuffer.
	1.	Ensure no desktop or X server is running:

sudo systemctl disable lightdm
sudo systemctl stop lightdm


	2.	Set environment for Qt EGLFS:

export QT_QPA_PLATFORM=eglfs
export QT_QPA_EGLFS_WIDTH=800
export QT_QPA_EGLFS_HEIGHT=480
export QT_QPA_EGLFS_HIDECURSOR=1
export QT_QPA_EGLFS_PHYSICAL_WIDTH=154  # adjust for your screen
export QT_QPA_EGLFS_PHYSICAL_HEIGHT=86


	3.	Run the app:

./build/piweather



It will start full-screen on the HDMI touch display and capture all input.

To auto-launch at boot, add this to /etc/rc.local before exit 0:

su - pi -c 'cd /home/pi/piweather && QT_QPA_PLATFORM=eglfs ./build/piweather &'


⸻

🌍 First Run (Address Entry)

On first launch:
	•	The app shows a touch keyboard and address field.
	•	Autocomplete suggestions are powered by OpenStreetMap Nominatim.
	•	After you pick a location, it’s stored locally (~/.local/share/piweather/settings.json).

Subsequent launches skip onboarding and display the weather directly.

⸻

☁️ Weather Data

Default provider: OpenWeatherMap

To use your own API key, edit ~/.local/share/piweather/settings.json or set the environment variable:

export OPENWEATHER_API_KEY="your_key_here"


⸻

⚡ Performance Tips for Pi Zero 2 W
	•	Runs fastest with the EGLFS backend (no compositor).
	•	Prefer SVG icons and compressed assets.
	•	Use performanceMode=true in settings to disable particles.
	•	Target frame rate: ~25–30 FPS.

⸻

🔒 Privacy

All location data is stored locally only.
Only anonymized coordinate requests are sent to geocoding and weather APIs.

⸻

📜 Licenses

Weather data courtesy of OpenWeatherMap
Geocoding powered by OpenStreetMap Nominatim

⸻

Example startup log (expected)

[INFO] PiWeather starting (EGLFS)
[INFO] Screen: 800x480
[INFO] Settings loaded (address=Paris, FR)
[INFO] WeatherProvider: Using OpenWeatherMap
[INFO] UI: MainPage.qml loaded


⸻

Enjoy this touch-friendly animated weather dashboard ☀️
