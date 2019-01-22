import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import org.tal.cm 1.0

Item {
    id: root
    anchors.fill: parent
    visible: true

    property alias position: player.position
    property alias playing: player.playing

    CMMediaPlayer {
        id: player

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

        Component.onCompleted: {
            setAudioSink(audioSink)
            files.model=player.getSongModel();
            player.refreshDatabase();
        }
    }

    CMAudioSink {
        id: audioSink
        // rate: 48000;
    }

    CMWavFileAudioSink {
        id: wavSink
        file: "audio-qml.wav"
    }

    RowLayout {
        id: control
        width: parent.width
        anchors.top: parent.top
        spacing: 4
        Button {
            text: "Play"
            enabled: !player.playing
            onClicked: {
                if (!player.play())
                    console.debug("Playback start failed")
            }
        }

        Button {
            text: "Pause"
            enabled: player.playing
            onClicked: player.pause();
        }

        Button {
            text: "Stop"
            enabled: player.playing
            onClicked: {
                player.stop();
            }
        }
        Button {
            text: "Prev Song"
            onClicked: {
                player.prepareNewSong();
                files.decrementCurrentIndex();
            }
        }
        Button {
            text: "Next Next"
            onClicked: {
                player.prepareNewSong();
                files.incrementCurrentIndex();
            }
        }
        Button {
            text: "Prev"
            enabled: player.track>1
            onClicked: {
                player.prevTrack();
            }
        }
        Button {
            text: "Next"
            enabled: player.tracks>1 && player.track<player.tracks
            onClicked: {
                player.nextTrack();
            }
        }

        Button {
            text: "WAV Sink"
            onClicked: {
                player.setAudioSink(wavSink)
            }
        }
        Button {
            text: "OutSink"
            onClicked: {
                player.setAudioSink(audioSink)
            }
        }


    }

    ColumnLayout {
        width: parent.width
        anchors.top: control.bottom
        anchors.bottom: parent.bottom

        ListView {
            id: files
            delegate: fileDelegate
            highlight: highlightDelegate
            highlightFollowsCurrentItem: true
            highlightMoveDuration: 500
            clip: true;
            width: parent.width
            height: parent.height
            model: player.getSongModel()
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
                    height: r.height;
                    RowLayout {
                        id: r
                        spacing: 8
                        anchors.margins: 16;
                        anchors.fill: parent
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
                    MouseArea {
                        anchors.fill: parent;
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
