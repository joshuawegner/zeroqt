import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

ApplicationWindow {
    id: window
    visible: true
    width: 800
    height: 480
    title: "MacroPad"

    Material.theme: Material.Dark
    Material.primary: Material.BlueGrey
    Material.accent: Material.Cyan

    // Stack view for navigation
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: mainPage
    }

    // Main macro grid page
    Component {
        id: mainPage
        
        Rectangle {
            color: Material.backgroundColor
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10
                
                // Header with status and settings
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    spacing: 10
                    
                    // Connection status indicator
                    Rectangle {
                        width: 16
                        height: 16
                        radius: 8
                        color: macroController.connected ? "#4CAF50" : "#F44336"
                        
                        SequentialAnimation on opacity {
                            running: !macroController.connected
                            loops: Animation.Infinite
                            NumberAnimation { to: 0.3; duration: 500 }
                            NumberAnimation { to: 1.0; duration: 500 }
                        }
                    }
                    
                    Label {
                        text: macroController.status
                        font.pixelSize: 14
                        color: Material.foreground
                        Layout.fillWidth: true
                    }
                    
                    // Pairing button
                    Button {
                        text: macroController.discoverable ? "Pairing..." : "Pair"
                        icon.name: "bluetooth"
                        Material.background: macroController.discoverable ? Material.Amber : Material.primary
                        onClicked: {
                            if (!macroController.discoverable) {
                                macroController.startPairing()
                            }
                        }
                    }
                    
                    // Settings button
                    Button {
                        text: "⚙️"
                        font.pixelSize: 20
                        onClicked: stackView.push(settingsPage)
                    }
                }
                
                // Macro grid
                MacroGrid {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    columns: macroController.columns
                    rows: macroController.rows
                    macros: macroController.macros
                    
                    onMacroClicked: function(macroId) {
                        macroController.executeMacro(macroId)
                    }
                }
            }
            
            // Error popup
            Popup {
                id: errorPopup
                x: (parent.width - width) / 2
                y: parent.height - height - 20
                width: parent.width * 0.8
                height: 60
                modal: false
                closePolicy: Popup.CloseOnPressOutside
                
                background: Rectangle {
                    color: "#D32F2F"
                    radius: 8
                }
                
                Label {
                    id: errorLabel
                    anchors.centerIn: parent
                    color: "white"
                    font.pixelSize: 14
                }
                
                Timer {
                    id: errorTimer
                    interval: 3000
                    onTriggered: errorPopup.close()
                }
            }
            
            // Macro executed feedback
            Popup {
                id: feedbackPopup
                x: (parent.width - width) / 2
                y: (parent.height - height) / 2
                width: 150
                height: 150
                modal: false
                closePolicy: Popup.CloseOnPressOutside
                
                background: Rectangle {
                    color: "#4CAF50"
                    radius: 16
                    opacity: 0.9
                }
                
                Label {
                    anchors.centerIn: parent
                    text: "✓"
                    font.pixelSize: 64
                    color: "white"
                }
                
                Timer {
                    id: feedbackTimer
                    interval: 300
                    onTriggered: feedbackPopup.close()
                }
            }
            
            Connections {
                target: macroController
                
                function onError(message) {
                    errorLabel.text = message
                    errorPopup.open()
                    errorTimer.restart()
                }
                
                function onMacroExecuted(macroId) {
                    feedbackPopup.open()
                    feedbackTimer.restart()
                }
            }
        }
    }

    // Settings page
    Component {
        id: settingsPage
        SettingsPage {
            onBackClicked: stackView.pop()
        }
    }
}

