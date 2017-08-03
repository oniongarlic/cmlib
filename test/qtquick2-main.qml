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

            if (mediaList.count>0) {
                player.prepareNewSong();
                mediaList.incrementCurrentIndex();
            }
        }
        onTracksChanged: console.debug(tracks)
        onTrackChanged: console.debug(track)
    }

    CMAudioSink {
        id: audioSink
    }

    CMWavFileAudioSink {
        id: wavSink
        file: "audio-qml.wav"
    }

    MediaList {
        id: mediaList
        anchors.fill: parent
        onFileSelected: {
            player.prepareNewSong();
            if (player.load(file)) {
                player.prepare();
                if (player.wasPlaying)
                    player.play();
            }
        }
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent
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
                    mediaList.decrementCurrentIndex();
                }
            }
            ToolButton {
                text: "Next Song"
                onClicked: {
                    player.prepareNewSong();
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
