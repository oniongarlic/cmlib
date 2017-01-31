#ifndef CMMEDIAPLAYER_H
#define CMMEDIAPLAYER_H

#include <QObject>
#include <QAudio>
#include "cmbaseaudiosource.h"
#include "cmbaseaudiosink.h"
#include "cmmediadecoder.h"

class CMMediaPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double position READ position NOTIFY positionChanged)
    Q_PROPERTY(double length READ length NOTIFY lengthChanged)

    Q_PROPERTY(int track READ track WRITE setTrack NOTIFY trackChanged)
    Q_PROPERTY(int tracks READ tracks NOTIFY tracksChanged)

    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)

public:
    explicit CMMediaPlayer(QObject *parent = 0);

    Q_INVOKABLE bool load(const QString file);
    bool load(const QByteArray &data, CMBaseAudioSource *source);
    Q_INVOKABLE bool play();
    Q_INVOKABLE bool stop();
    Q_INVOKABLE bool pause();
    Q_INVOKABLE bool prepare();

    Q_INVOKABLE bool prevTrack();
    Q_INVOKABLE bool nextTrack();

    Q_INVOKABLE bool setTrack(quint16 track);
    Q_INVOKABLE void setPlaytime(quint64 ms);

    Q_INVOKABLE bool setAudioSink(CMBaseAudioSink *sink);

    QAudio::State getState();

    double position() const
    {
        return m_position;
    }

    double length() const
    {
        return m_length;
    }

    int tracks() const
    {
        return m_tracks;
    }

    int track() const
    {
        return m_track;
    }

    bool playing() const
    {
        return m_playing;
    }

signals:
    void metadata(QVariantHash meta);
    void positionChanged(quint64 position);
    void lengthChanged(double length);
    void tracksChanged(int tracks);
    void trackChanged(int track);
    void eot();

    void playingChanged(bool playing);

protected slots:
    void decoderMetadata(QVariantHash meta);
    void decoderEOT();
    void sinkPosition(quint64 pos);
    void sinkState(QAudio::State state);

public slots:

private:    
    CMBaseAudioSource *m_source;
    CMBaseAudioSink *m_sink;
    QAudio::State m_state;
    CMMediaDecoder m_dec;
    double m_position;
    double m_length;
    quint64 m_playtime;
    int m_tracks;
    int m_track;
    bool m_playing;
};

#endif // CMMEDIAPLAYER_H
