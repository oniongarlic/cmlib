import QtQuick 1.1
import org.tal.cm 1.0
import "qtquick1"

Rectangle {
    id: root
    width: 1024
    height: 600
    gradient: Gradient {
        GradientStop {
            position: 0.00;
            color: "#ffffff";
        }
        GradientStop {
            position: 1.00;
            color: "#606bff";
        }
    }
    visible: true

    Player {
        anchors.fill: parent
    }
}
