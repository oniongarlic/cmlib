import QtQuick 2.6
import org.tal.cm 1.0
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtPositioning 5.5
import QtQuick.Controls 1.4

Window {
    id: root
    visible: true
    width: 1024
    height: 600
    title: qsTr("CMLibTest")

    Player {
        anchors.fill: parent
    }

}
