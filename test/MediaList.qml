import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.4

ListView {
    id: files
    delegate: fileDelegate
    highlight: highlightDelegate
    highlightFollowsCurrentItem: true
    highlightMoveDuration: 500
    clip: true;
    focus: true
    currentIndex: -1
    headerPositioning: ListView.OverlayHeader

    property int scrollIndex: 0

    section.property: "title"
    section.criteria: ViewSection.FirstCharacter
    section.delegate: sectionHeading

    ScrollBar.vertical: ScrollBar {}

    //flickableDirection: Flickable.HorizontalAndVerticalFlick

    signal fileSelected(string file)
    signal playSelected();
    signal pressAndHold(int index)

    onCurrentSectionChanged: console.debug("Section:"+currentSection)

    onCurrentIndexChanged: {
        var data=player.get(currentIndex)
        console.debug(data)
        fileSelected(data.file)
        scrollIndex=currentIndex
    }

    Keys.onSpacePressed: {
        scrollRelative(25)
    }

    function scrollRelative(rel) {
        var ni=scrollIndex+rel;
        if (ni>count)
            ni=count;
        positionViewAtIndex(ni, ListView.Center);
        scrollIndex=ni;
    }

    Keys.onPressed: {
        switch (event.key) {
        case Qt.Key_PageDown:
            scrollRelative(60);
            event.accepted = true;
            break;

        case Qt.Key_PageUp:
            scrollRelative(-60);
            event.accepted = true;
            break;
        case Qt.Key_Home:
            positionViewAtBeginning();
            event.accepted = true;
            break;
        case Qt.Key_End:
            positionViewAtEnd();
            event.accepted = true;
            break;
        }
    }

    property int titleWidth;

    function setSelection(index)
    {
        if (index!=currentIndex) {
            files.currentIndex=index
            var data=player.get(currentIndex)
            console.debug(data)
            fileSelected(data.file)
        }
    }

    function playSelection(index)
    {
        if (index!=currentIndex)
            setSelection(index)
        playSelected();
    }

    Component {
        id: sectionHeading
        Rectangle {
            width: parent.width
            height: childrenRect.height
            color: "lightsteelblue"
            Text {
                text: section
                font.bold: true
                font.pixelSize: 20
            }
        }
    }

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
                    console.debug("Clicked: "+index);
                    setSelection(index)
                }
                onDoubleClicked: {
                    console.debug("DoubleClicked"+index);
                    playSelection(index)
                }
                onPressAndHold: {
                    files.pressAndHold(index);
                }
            }
        }
    }
}


