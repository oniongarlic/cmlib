import QtQuick 1.1

Rectangle {
    id: root
    width: 1024
    height: 600

    Row {
        id: control
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

        Button {
            title: "Scan"
            onClicked: {
                _player.stop();
            }
        }
    }

    Column {
        width: parent.width
        anchors.top: control.bottom
        anchors.bottom: parent.bottom
        ListView {
            id: files
            delegate: fileDelegate
            clip: true;
            width: parent.width
            height: parent.height
            model: _files

            signal inputSelected(variant input);
            property int currentInput: -1;

            Component {
                id: fileDelegate
                Rectangle {
                    // color: model.input_id==inputs.currentInput ? "#e5e5e5" : "#ffffff";
                    width: parent.width;
                    height: txt.height;
                    MouseArea {
                        anchors.fill: parent;
                        Text {
                            id: txt
                            text: display;
                            anchors.margins: 8;
                            font.pointSize: 12
                        }
                        onClicked: {
                            console.debug("File: "+model.display);

                        }
                    }
                }
            }
        }
    }

}
