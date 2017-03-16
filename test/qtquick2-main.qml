import QtQuick 2.8
import org.tal.cm 1.0
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.1

ApplicationWindow {
    id: root
    visible: true
    width: 1024
    height: 600
    title: qsTr("CMLibTest")

    Player {
        anchors.fill: parent
    }
}
