import QtQuick 2.8
import QtQuick.Layouts 1.1
import org.tal.cm 1.0

ListView {
    id: files
    delegate: fileDelegate
    highlight: highlightDelegate
    highlightFollowsCurrentItem: true
    highlightMoveDuration: 500
    clip: true;
    model: _files
    currentIndex: -1
    headerPositioning: ListView.PullBackHeader

    //flickableDirection: Flickable.HorizontalAndVerticalFlick

    signal fileSelected(string file)

    onCurrentIndexChanged: {
        var data=model.get(currentIndex)
        console.debug(data)
        fileSelected(data.file)
    }

    property int titleWidth;

    header: Component {
        id: headerComponent
        RowLayout {
            width: parent.width
            spacing: 8
            Text {
                id: headerTitle
                text: "Song title"
                font.pixelSize: 28
                Layout.fillWidth: true
                Component.onCompleted: titleWidth=width;
            }
            Text {
                id: headerType
                text: "Type"
                font.pixelSize: 28
                Layout.fillWidth: false;
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
            width: parent.width;
            height: r.height;
            MouseArea {
                anchors.fill: parent;
                RowLayout {
                    id: r
                    anchors.margins: 16;
                    Text {
                        id: txt
                        text: model.filename;
                        font.pixelSize: 24
                        Layout.preferredWidth: titleWidth
                    }
                    Text {
                        id: type
                        text: model.type
                        font.pixelSize: 22
                    }
                }
                onClicked: {
                    console.debug("File: "+model.file);
                    files.currentIndex=index
                }
            }
        }
    }
}


