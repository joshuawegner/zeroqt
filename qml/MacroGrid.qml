import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

/**
 * MacroGrid - Grid layout for macro buttons
 */
Item {
    id: root
    
    property int columns: 4
    property int rows: 3
    property var macros: []
    property string executingMacro: ""
    
    signal macroClicked(string macroId)
    
    GridView {
        id: gridView
        anchors.fill: parent
        anchors.margins: 5
        
        cellWidth: width / columns
        cellHeight: height / rows
        
        model: macros
        
        delegate: Item {
            width: gridView.cellWidth
            height: gridView.cellHeight
            
            MacroButton {
                anchors.fill: parent
                anchors.margins: 5
                
                macroId: modelData.id || ""
                macroName: modelData.name || ""
                macroIcon: modelData.icon || "⚡"
                macroColor: modelData.color || "#666666"
                isExecuting: executingMacro === macroId
                
                onClicked: {
                    root.macroClicked(macroId)
                }
            }
        }
        
        // Smooth scrolling for touch
        flickDeceleration: 1500
        maximumFlickVelocity: 2500
        
        // Bounce effect at edges
        boundsBehavior: Flickable.DragAndOvershootBounds
    }
    
    // Empty state
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        visible: macros.length === 0
        
        Column {
            anchors.centerIn: parent
            spacing: 20
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "🎹"
                font.pixelSize: 64
            }
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "No macros configured"
                font.pixelSize: 18
                color: Material.foreground
            }
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Go to Settings to add macros"
                font.pixelSize: 14
                color: Material.hintTextColor
            }
        }
    }
    
    // Connection status overlay when not connected
    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.7)
        visible: !macroController.connected
        
        MouseArea {
            anchors.fill: parent
            // Block touches when not connected
        }
        
        Column {
            anchors.centerIn: parent
            spacing: 20
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "📱"
                font.pixelSize: 64
            }
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Not Connected"
                font.pixelSize: 24
                font.bold: true
                color: "white"
            }
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Pair with a device to use macros"
                font.pixelSize: 14
                color: "#AAAAAA"
            }
            
            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: macroController.discoverable ? "Searching..." : "Start Pairing"
                Material.background: Material.Cyan
                enabled: !macroController.discoverable
                
                onClicked: macroController.startPairing()
            }
        }
    }
}
