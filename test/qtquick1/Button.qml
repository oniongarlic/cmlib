import QtQuick 1.1

Rectangle {
    id: btn
    width: abc.width+16
    height: abc.height+18
    color: ma.pressed ? "#80E980" : !enabled ? "#C0C0C0" : "#C0F0C0"
    property alias title: abc.text
    property bool enabled: true
    Text {
        id: abc
        anchors.centerIn: parent;
    }
    MouseArea {
        id: ma
        anchors.fill: parent
        enabled: btn.enabled
        onClicked: {
           btn.clicked();
        }
    }
    signal clicked();
}
