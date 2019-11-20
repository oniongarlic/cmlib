import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import org.tal.cm 1.0

import "qtquick2"

ApplicationWindow {
    id: root
    visible: true
    width: 1024
    height: 600
    title: qsTr("CMLibTest")

    CMMediaPlayer {
        id: player
        Component.onCompleted: setAudioSink(audioSink)

        property bool wasPlaying: false

        property string author;
        property string title;

        onWasPlayingChanged: console.debug("WasPlaying: "+wasPlaying)

        function prepareNewSong() {
            console.debug("Preparing next song for playback")
            if (player.playing) {
                console.debug("Stopping current playback")
                player.stop();
                player.wasPlaying=true;
            } else {
                player.wasPlaying=false;
            }
        }

        onMetadata: {
            title=meta.title;
            author=meta.author;
        }

        onEot: {
            console.debug("EOT!")
            player.stop();

            if (mediaList.count>0) {
                player.prepareNewSong();
                mediaList.incrementCurrentIndex();
            }
        }
        onTracksChanged: console.debug("Tracks: "+tracks)
        onTrackChanged: {
            console.debug("Track:"+track)
        }
    }

    CMAudioSink {
        id: audioSink
    }

    CMWavFileAudioSink {
        id: wavSink
        file: "audio-"+player.track+"-"+player.title+"-_-"+player.author+"-qml.wav"
    }

    MediaList {
        id: mediaList
        anchors.fill: parent
        model: player.getSongModel()

        property bool prepared: false

        onFileSelected: {
            forceActiveFocus();
            player.prepareNewSong();
            if (player.load(file)) {
                prepared=player.prepare();
                if (player.wasPlaying) {
                    console.debug("We where playing, starting playback of loaded file")
                    player.play();
                }
            }
        }
        onPlaySelected: {
            if (prepared && !player.playing)
                player.play();
        }

        onPressAndHold: {
            contextMenu.open();
        }
    }

    Menu {
        id: contextMenu
        MenuItem {
            text: "Details..."
        }
    }


    footer: ToolBar {
        RowLayout {
            anchors.fill: parent

            Text {
                text: player.title
                font.bold: true
                Layout.fillWidth: true
            }

            Text {
                text: player.author
                font.italic: true
                Layout.fillWidth: true
            }

            Text {
                text:player.position/1000;
                horizontalAlignment: Text.AlignHCenter
                Layout.minimumWidth: 100
                Layout.maximumWidth: 200
            }

            Text {
                text: player.track + " / " + player.tracks
                Layout.fillWidth: true
            }

            ToolSeparator {

            }

            ToolButton {
                text: "WAV Sink"
                onClicked: {
                    player.setAudioSink(wavSink)
                }
            }
            ToolButton {
                text: "OutSink"
                onClicked: {
                    player.setAudioSink(audioSink)
                }
            }
        }
    }

    Connections {
        target: player.getMediaScanner()
        onScanning: {
            // console.debug("Scanning "+path)
            scanning.text=path;
        }
        onScanningDone: {
            console.debug("Done!")
            scanning.text='';
        }
        onScanningChanged: {
            console.debug("Scanning: "+scanning)
            if (scanning)
                dialogScanning.open();
            else
                dialogScanning.close();
        }
    }

    header: ToolBar {
        RowLayout {
            ToolButton {
                text: "Play"
                enabled: !player.playing && player.tracks>0
                onClicked: {
                    if (!player.play())
                        console.debug("Playback start failed")
                }
            }

            ToolButton {
                text: "Pause"
                enabled: player.playing
                onClicked: player.pause();
            }

            ToolButton {
                text: "Stop"
                enabled: player.playing
                onClicked: {
                    player.stop();
                }
            }
            ToolButton {
                text: "Prev Song"
                onClicked: {
                    mediaList.decrementCurrentIndex();
                }
            }
            ToolButton {
                text: "Next Song"
                onClicked: {
                    mediaList.incrementCurrentIndex();
                }
            }
            ToolButton {
                text: "Prev"
                enabled: player.track>1
                onClicked: {
                    player.prevTrack();
                }
            }
            ToolButton {
                text: "Next"
                enabled: player.tracks>1 && player.track<player.tracks
                onClicked: {
                    player.nextTrack();
                }
            }

            ToolSeparator {

            }

            ToolButton {
                text: "Scan"
                onClicked: player.getMediaScanner().scanAsync(true)
            }

            ToolSeparator {

            }

            ToolButton {
                text: "Search"
                onClicked: {
                    mainDrawer.open()
                    searchText.forceActiveFocus();
                    searchText.selectAll();
                }
            }

            ToolSeparator {

            }

            ToolButton {
                text: "WAV Sink"
                onClicked: {
                    player.setAudioSink(wavSink)
                }
            }
            ToolButton {
                text: "OutSink"
                onClicked: {
                    player.setAudioSink(audioSink)
                }
            }

            ToolSeparator {

            }

            ToolButton {
                text: "Quit"
                onClicked: {
                    Qt.quit()
                }
            }
        }
    }

    Dialog {
        id: dialogScanning
        modal: true
        title: "Scanning for media..."
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: parent.width/2
        standardButtons: Dialog.Cancel
        ColumnLayout {
            anchors.fill: parent
            spacing: 16
            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: player.getMediaScanner().scanning
            }
            Text {
                id: scanning
                text: ""
            }
        }
        onRejected: {
            player.getMediaScanner().scanAsyncCancel();
        }
    }

    Drawer {
        id: mainDrawer
        width: root.width
        edge: Qt.BottomEdge
        ColumnLayout {
            //anchors.fill: parent
            spacing: 16
            Layout.alignment: Qt.AlignTop
            Layout.margins: 16
            Text {
                id: name
                text: qsTr("Search")
            }
            RowLayout {
                TextField {
                    id: searchText
                    Layout.fillWidth: true
                    placeholderText: "Search tracks..."

                    onAccepted: {
                        player.setFilter(text)
                    }
                }
                Button {
                    text: "Clear"
                    onClicked: {
                        searchText.text=''
                        player.setFilter('')
                    }
                }
            }
        }
    }
}
