import QtQuick 2.8
import org.tal.cm 1.0
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.1

ApplicationWindow {
    id: root
    visible: true
    width: 1024
    height: 600
    title: qsTr("CMLibTest")

    CMMediaPlayer {
        id: player
        Component.onCompleted: setAudioSink(audioSink)

        onMetadata: {
            console.debug(meta.title)
        }
        onEot: {
            console.debug("EOT!")
            player.stop();
            //files.incrementCurrentIndex();
            //player.play();
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
            if (player.load(file))
                player.prepare();
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
            anchors.fill: parent
            ToolButton {
                text: "Play"
                enabled: !player.playing
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
                text: "Prev"
                enabled: player.track>1
                onClicked: {
                    player.prevTrack();
                }
            }
            ToolButton {
                text: "Next"
                enabled: player.tracks>1
                onClicked: {
                    player.nextTrack();
                }
            }
        }
    }

    header: ToolBar {
        RowLayout {

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
}
