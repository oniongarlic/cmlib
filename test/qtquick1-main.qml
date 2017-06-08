import QtQuick 1.1
import org.tal.cm 1.0
import "qtquick1"

Rectangle {
    id: root
    width: 1024
    height: 600
    visible: true

    Player {
        anchors.fill: parent
    }
}
