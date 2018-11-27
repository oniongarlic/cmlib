import QtQuick 2.9
import QtQuick.Layouts 1.4

ListView {
    id: files
    delegate: fileDelegate
    highlight: highlightDelegate
    highlightFollowsCurrentItem: true
    highlightMoveDuration: 500
    clip: true;
    currentIndex: -1
    headerPositioning: ListView.PullBackHeader

    //flickableDirection: Flickable.HorizontalAndVerticalFlick

    signal fileSelected(string file)
    signal pressAndHold(int index)

    onCurrentIndexChanged: {
        var data=player.get(currentIndex)
        console.debug(data)
        fileSelected(data.file)
    }

    property int titleWidth;

    header: Component {
        id: headerComponent
        Rectangle {
            width: parent.width
            height: rl.height
            color: "lightgrey"
            z: 3
            RowLayout {
                id: rl
                width: parent.width
                spacing: 8
                Text {
                    id: headerTitle
                    text: "Song title"
                    font.pixelSize: 26
                    Layout.fillWidth: true
                    Component.onCompleted: titleWidth=width;
                }
                Text {
                    id: headerType
                    text: "Type"
                    font.pixelSize: 26
                    Layout.fillWidth: false;
                }
            }
        }
    }

    Component {
        id: highlightDelegate
        Rectangle {
            color: "#70d370"
            width: parent.width
            height: 20
        }
    }

    Component {
        id: fileDelegate
        Item {
            height: r.height;
            width: parent.width
            RowLayout {
                id: r
                width: parent.width;
                spacing: 16
                Text {
                    id: txt
                    text: model.filename;
                    font.pixelSize: 24
                    Layout.preferredWidth: titleWidth
                    Layout.fillWidth: true
                }
                Text {
                    id: type
                    color: "#ec0f0f"
                    text: model.type
                    // visible: false
                    font.pixelSize: 22
                    Layout.fillWidth: false
                }
            }
            MouseArea {
                anchors.fill: parent;                
                onClicked: {
                    console.debug("File: "+model.file);
                    files.currentIndex=index
                }
                onPressAndHold: {
                    files.pressAndHold(index);
                }
            }
        }
    }
}


