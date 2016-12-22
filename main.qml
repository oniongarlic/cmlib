import QtQuick 1.1

Rectangle {
    id: root
    width: 1024
    height: 600

    Row {
        width: parent.width
        anchors.top: parent.top
        Button {
            title: "Play"
            onClicked: _player.play();
        }

        Button {
            title: "Pause"
            onClicked: _player.pause();
        }

        Button {
            title: "Resume"
            onClicked: _player.resume();
        }

        Button {
            title: "Stop"
            onClicked: {
                _player.stop();
            }
        }
    }
}
