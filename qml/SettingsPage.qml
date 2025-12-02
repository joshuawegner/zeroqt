import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

/**
 * SettingsPage - Configuration page for the macro pad
 */
Rectangle {
    id: root
    color: Material.backgroundColor
    
    signal backClicked()
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Button {
                text: "←"
                font.pixelSize: 24
                flat: true
                onClicked: root.backClicked()
            }
            
            Label {
                text: "Settings"
                font.pixelSize: 24
                font.bold: true
                Layout.fillWidth: true
            }
        }
        
        // Scrollable settings content
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            ColumnLayout {
                width: parent.width
                spacing: 20
                
                // Bluetooth Section
                GroupBox {
                    Layout.fillWidth: true
                    title: "Bluetooth"
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 10
                        
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Label {
                                text: "Status:"
                                Layout.preferredWidth: 120
                            }
                            
                            Rectangle {
                                width: 12
                                height: 12
                                radius: 6
                                color: macroController.connected ? "#4CAF50" : "#F44336"
                            }
                            
                            Label {
                                text: macroController.connected ? "Connected" : "Disconnected"
                                Layout.fillWidth: true
                            }
                        }
                        
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Label {
                                text: "Device Name:"
                                Layout.preferredWidth: 120
                            }
                            
                            TextField {
                                id: deviceNameField
                                Layout.fillWidth: true
                                text: macroController.deviceName
                                placeholderText: "MacroPad"
                                
                                onEditingFinished: {
                                    macroController.deviceName = text
                                }
                            }
                        }
                        
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10
                            
                            Button {
                                text: macroController.discoverable ? "Pairing Mode Active" : "Start Pairing"
                                Material.background: macroController.discoverable ? Material.Amber : Material.primary
                                enabled: !macroController.discoverable && !macroController.connected
                                
                                onClicked: macroController.startPairing()
                            }
                            
                            Button {
                                text: "Disconnect"
                                Material.background: Material.Red
                                enabled: macroController.connected
                                
                                onClicked: macroController.disconnect()
                            }
                        }
                    }
                }
                
                // Grid Layout Section
                GroupBox {
                    Layout.fillWidth: true
                    title: "Grid Layout"
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 10
                        
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Label {
                                text: "Columns:"
                                Layout.preferredWidth: 120
                            }
                            
                            Slider {
                                id: columnsSlider
                                Layout.fillWidth: true
                                from: 2
                                to: 6
                                stepSize: 1
                                value: macroController.columns
                                
                                onValueChanged: {
                                    if (pressed) return
                                    macroController.setGridLayout(value, rowsSlider.value)
                                }
                                
                                onPressedChanged: {
                                    if (!pressed) {
                                        macroController.setGridLayout(value, rowsSlider.value)
                                    }
                                }
                            }
                            
                            Label {
                                text: columnsSlider.value.toFixed(0)
                                Layout.preferredWidth: 30
                            }
                        }
                        
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Label {
                                text: "Rows:"
                                Layout.preferredWidth: 120
                            }
                            
                            Slider {
                                id: rowsSlider
                                Layout.fillWidth: true
                                from: 2
                                to: 5
                                stepSize: 1
                                value: macroController.rows
                                
                                onValueChanged: {
                                    if (pressed) return
                                    macroController.setGridLayout(columnsSlider.value, value)
                                }
                                
                                onPressedChanged: {
                                    if (!pressed) {
                                        macroController.setGridLayout(columnsSlider.value, value)
                                    }
                                }
                            }
                            
                            Label {
                                text: rowsSlider.value.toFixed(0)
                                Layout.preferredWidth: 30
                            }
                        }
                        
                        Label {
                            text: "Grid: " + columnsSlider.value.toFixed(0) + " x " + rowsSlider.value.toFixed(0) + " = " + (columnsSlider.value * rowsSlider.value).toFixed(0) + " buttons"
                            font.pixelSize: 12
                            color: Material.hintTextColor
                        }
                    }
                }
                
                // Macros Section
                GroupBox {
                    Layout.fillWidth: true
                    title: "Macros (" + macroController.macros.length + ")"
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 10
                        
                        Repeater {
                            model: macroController.macros
                            
                            delegate: Rectangle {
                                Layout.fillWidth: true
                                height: 50
                                radius: 8
                                color: modelData.color || "#666666"
                                
                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    spacing: 10
                                    
                                    Text {
                                        text: modelData.icon || "⚡"
                                        font.pixelSize: 24
                                    }
                                    
                                    Label {
                                        text: modelData.name || "Unnamed"
                                        font.bold: true
                                        color: "white"
                                        Layout.fillWidth: true
                                    }
                                    
                                    Button {
                                        text: "🗑️"
                                        flat: true
                                        onClicked: {
                                            deleteConfirmDialog.macroId = modelData.id
                                            deleteConfirmDialog.macroName = modelData.name
                                            deleteConfirmDialog.open()
                                        }
                                    }
                                }
                            }
                        }
                        
                        Button {
                            Layout.fillWidth: true
                            text: "Reset to Defaults"
                            Material.background: Material.Orange
                            
                            onClicked: resetConfirmDialog.open()
                        }
                    }
                }
                
                // About Section
                GroupBox {
                    Layout.fillWidth: true
                    title: "About"
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 5
                        
                        Label {
                            text: "MacroPad v1.0"
                            font.bold: true
                        }
                        
                        Label {
                            text: "Bluetooth HID Macro Keyboard"
                            color: Material.hintTextColor
                        }
                        
                        Label {
                            text: "For Raspberry Pi Zero with touchscreen"
                            color: Material.hintTextColor
                            font.pixelSize: 12
                        }
                    }
                }
                
                // Spacer
                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }
    
    // Delete confirmation dialog
    Dialog {
        id: deleteConfirmDialog
        property string macroId: ""
        property string macroName: ""
        
        title: "Delete Macro"
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Yes | Dialog.No
        
        Label {
            text: "Delete macro '" + deleteConfirmDialog.macroName + "'?"
        }
        
        onAccepted: {
            macroController.removeMacro(macroId)
        }
    }
    
    // Reset confirmation dialog
    Dialog {
        id: resetConfirmDialog
        title: "Reset to Defaults"
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Yes | Dialog.No
        
        Label {
            text: "This will delete all custom macros and restore defaults.\nAre you sure?"
        }
        
        onAccepted: {
            macroController.reloadConfig()
        }
    }
}
