import QtQuick 2.12
import QtQuick.Layouts 1.4

ListView {
    id: files
    delegate: fileDelegate
    highlight: highlightDelegate
    highlightFollowsCurrentItem: true
    highlightMoveDuration: 500
    clip: true;
    currentIndex: -1
    headerPositioning: ListView.OverlayHeader

    //flickableDirection: Flickable.HorizontalAndVerticalFlick

    signal fileSelected(string file)
    signal pressAndHold(int index)

    onCurrentIndexChanged: {
        var data=player.get(currentIndex)
        console.debug(data)
        fileSelected(data.file)
    }

    property int titleWidth;

    Component {
        id: headerComponent
        RowLayout {
            id: headerRow
            spacing: 1
            z: 2
            width: parent.width
            function itemAt(index) { return r2.itemAt(index) }

            signal clicked(int index)

            Repeater {
                id: r2
                model: lm
                Text {
                    text: name
                    font.bold: true
                    font.pixelSize: 18
                    padding: 2
                    //background: Rectangle { color: "grey" }
                    Layout.fillWidth: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            console.debug(index)
                            headerRow.clicked(index)
                        }
                    }
                }
            }
            ListModel {
                id: lm
                ListElement { name: "Name"; sort: "name"; }
                ListElement { name: "Type"; sort: "type" }
            }
        }

    }

    header: headerComponent

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
                    //Layout.preferredWidth: titleWidth
                    Layout.preferredWidth: files.headerItem.itemAt(0).width
                    //Layout.fillWidth: true
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


