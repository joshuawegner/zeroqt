import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

ApplicationWindow {
    visible: true
    width: 800
    height: 480
    title: "PiWeather"

    Material.theme: Material.Dark
    Material.primary: Material.Blue
    Material.accent: Material.LightBlue

    Rectangle {
        anchors.fill: parent
        color: Material.backgroundColor

        Text {
            anchors.centerIn: parent
            text: "PiWeather App – Startup OK"
            font.pixelSize: 32
            color: Material.foreground
        }
    }
}
