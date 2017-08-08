import QtQuick 2.8
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.1

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

        function prepareNewSong() {
            if (player.playing) {
                console.debug("Stopping current playback")
                player.stop();
                player.wasPlaying=true;
            } else {
                player.wasPlaying=false;
            }
        }

        onMetadata: {
            console.debug(meta.title)
        }
        onEot: {
            console.debug("EOT!")
            player.stop();
            if (files.count>0) {
                player.prepareNewSong();
                files.incrementCurrentIndex();
            }
        }
        onTracksChanged: console.debug(tracks)
        onTrackChanged: console.debug(track)
    }

    CMAudioSink {
        id: audioSink
        // rate: 48000;
    }

    Connections {
        target: _scanner
        onScanning: {
            console.debug("Scanning "+path)
        }
        onScanningDone: {
            console.debug("Done!")
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
                    player.prepareNewSong();
                    files.decrementCurrentIndex();
                }
            }
            ToolButton {
                text: "Next Next"
                onClicked: {
                    player.prepareNewSong();
                    files.incrementCurrentIndex();
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
                onClicked: _scanner.scanAsync()
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

    Item {
        anchors.fill: parent
        ColumnLayout {
            anchors.fill: parent
            ListView {
                id: files
                delegate: fileDelegate
                highlight: highlightDelegate
                highlightFollowsCurrentItem: true
                highlightMoveDuration: 500
                clip: true;
                Layout.fillHeight: true
                Layout.fillWidth: true
                model: _files
                currentIndex: -1

                //flickableDirection: Flickable.HorizontalAndVerticalFlick

                onCurrentIndexChanged: {
                    var data=model.get(currentIndex)
                    console.debug(data)
                    if (player.load(data.file)) {
                        player.prepare();
                        if (player.wasPlaying)
                            player.play();
                    }
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
                            Row {
                                id: r
                                anchors.margins: 16;
                                Text {
                                    id: txt
                                    text: model.title;
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
                                player.prepareNewSong();
                                files.currentIndex=index;
                            }
                        }
                    }
                }
            }
        }
    }

    footer: ToolBar {
        RowLayout {
            Text {
                id: posText
                width: 80
                height: parent.height
                text: player.position/1000;
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    Drawer {
        id: mainDrawer
        height: root.height
        width: root.width/1.5
        dragMargin: rootStack.depth > 1 ? 0 : Qt.styleHints.startDragDistance
        ColumnLayout {
            anchors.fill: parent
            spacing: 16
        }
    }
}
