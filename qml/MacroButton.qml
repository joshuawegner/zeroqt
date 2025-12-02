import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

/**
 * MacroButton - A single touchable macro button
 */
Rectangle {
    id: root
    
    property string macroId: ""
    property string macroName: ""
    property string macroIcon: ""
    property string macroColor: "#666666"
    property bool isExecuting: false
    
    signal clicked()
    
    width: 100
    height: 100
    radius: 12
    color: mouseArea.pressed ? Qt.darker(macroColor, 1.3) : macroColor
    
    // Subtle gradient overlay
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(1, 1, 1, 0.1) }
            GradientStop { position: 1.0; color: Qt.rgba(0, 0, 0, 0.2) }
        }
    }
    
    // Border
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: "transparent"
        border.width: 2
        border.color: Qt.rgba(1, 1, 1, 0.2)
    }
    
    // Content
    Column {
        anchors.centerIn: parent
        spacing: 8
        
        // Icon
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: macroIcon
            font.pixelSize: 32
            horizontalAlignment: Text.AlignHCenter
        }
        
        // Name
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: macroName
            font.pixelSize: 12
            font.bold: true
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            width: root.width - 16
            elide: Text.ElideRight
        }
    }
    
    // Executing indicator
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: "white"
        opacity: isExecuting ? 0.3 : 0
        
        SequentialAnimation on opacity {
            running: isExecuting
            loops: Animation.Infinite
            NumberAnimation { to: 0.3; duration: 200 }
            NumberAnimation { to: 0.0; duration: 200 }
        }
    }
    
    // Touch area
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: root.clicked()
        
        // Press feedback
        onPressed: {
            pressAnim.start()
        }
    }
    
    // Press animation
    SequentialAnimation {
        id: pressAnim
        
        NumberAnimation {
            target: root
            property: "scale"
            to: 0.95
            duration: 50
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            target: root
            property: "scale"
            to: 1.0
            duration: 100
            easing.type: Easing.OutQuad
        }
    }
    
    // Ripple effect
    Rectangle {
        id: ripple
        anchors.centerIn: parent
        width: 0
        height: width
        radius: width / 2
        color: Qt.rgba(1, 1, 1, 0.3)
        opacity: 0
        
        SequentialAnimation {
            id: rippleAnim
            
            ParallelAnimation {
                NumberAnimation {
                    target: ripple
                    property: "width"
                    from: 0
                    to: root.width * 1.5
                    duration: 300
                }
                NumberAnimation {
                    target: ripple
                    property: "opacity"
                    from: 0.5
                    to: 0
                    duration: 300
                }
            }
        }
    }
    
    Connections {
        target: mouseArea
        function onPressed() {
            rippleAnim.start()
        }
    }
}
