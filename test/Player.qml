import QtQuick 2.6
import org.tal.cm 1.0

Rectangle {
    id: root
    width: 600
    height: 600
    visible: true

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
            title: "Prev"
            enabled: player.track>1
            onClicked: {
                player.prevTrack();
            }
        }
        Button {
            title: "Next"
            enabled: player.tracks>1
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

        Text {
            id: posText
            width: 80
            height: parent.height
            text:player.position/1000;
            horizontalAlignment: Text.AlignHCenter
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
            clip: true;
            width: parent.width
            height: parent.height
            model: _files
            currentIndex: -1

            //flickableDirection: Flickable.HorizontalAndVerticalFlick

            onCurrentIndexChanged: {
                //var data=model.get(currentIndex)
                //console.debug(data)
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
                    height: txt.height;
                    MouseArea {
                        anchors.fill: parent;
                        Text {
                            id: txt
                            text: display;
                            anchors.margins: 16;
                            font.pixelSize: 18
                        }
                        onClicked: {
                            console.debug("File: "+model.display);
                            files.currentIndex=index
                            if (player.load(model.display))
                                player.prepare();
                        }
                    }
                }
            }
        }
    }

}
