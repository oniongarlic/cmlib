import QtQuick 1.1
import org.tal.cm 1.0

Item {
    id: root
    anchors.fill: parent
    visible: true

    property alias position: player.position

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
        rate: 48000;
    }

    CMWavFileAudioSink {
        id: wavSink
        file: "audio-qml.wav"
    }

    Row {
        id: control
        width: parent.width
        anchors.top: parent.top
        spacing: 4
        Button {
            title: "Play"
            enabled: !player.playing
            onClicked: {
                if (!player.play())
                    console.debug("Playback start failed")
            }
        }

        Button {
            title: "Pause"
            enabled: player.playing
            onClicked: player.pause();
        }

        Button {
            title: "Stop"
            enabled: player.playing
            onClicked: {
                player.stop();
            }
        }
        Button {
            title: "Prev Song"
            onClicked: {
                player.prepareNewSong();
                files.decrementCurrentIndex();
            }
        }
        Button {
            title: "Next Next"
            onClicked: {
                player.prepareNewSong();
                files.incrementCurrentIndex();
            }
        }
        Button {
            title: "Prev"
            enabled: player.track>1
            onClicked: {
                player.prevTrack();
            }
        }
        Button {
            title: "Next"
            enabled: player.tracks>1 && player.track<player.tracks
            onClicked: {
                player.nextTrack();
            }
        }

        Button {
            title: "WAV Sink"
            onClicked: {
                player.setAudioSink(wavSink)
            }
        }
        Button {
            title: "OutSink"
            onClicked: {
                player.setAudioSink(audioSink)
            }
        }


    }

    Column {
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
                            player.prepareNewSong();
                            files.currentIndex=index;
                        }
                    }
                }
            }
        }
    }
}
