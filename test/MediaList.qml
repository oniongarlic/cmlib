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

    //flickableDirection: Flickable.HorizontalAndVerticalFlick

    signal fileSelected(string file)

    onCurrentIndexChanged: {
        var data=model.get(currentIndex)
        console.debug(data)
    }

    Component {
        id: highlightDelegate
        Rectangle {
            color: "green"
            width: parent.width
            height: 20
        }
    }

    Component {
        id: fileDelegate
        Item {
            //color: files.currentIndex==inputs.currentInput ? "#e5e5e5" : "#ffffff";
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
                    }
                    Text {
                        id: type
                        text: model.type
                        color: "red"
                    }
                }
                onClicked: {
                    console.debug("File: "+model.file);
                    files.currentIndex=index
                    fileSelected(model.file)
                }
            }
        }
    }
}


