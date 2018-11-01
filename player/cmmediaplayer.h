#ifndef CMMEDIAPLAYER_H
#define CMMEDIAPLAYER_H

#include <QObject>
#include <QAudio>
#include <QSortFilterProxyModel>
#include "cmbaseaudiosource.h"
#include "cmbaseaudiosink.h"
#include "cmmediadecoder.h"
#include "scanner/cmmediascanner.h"

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

    Q_INVOKABLE bool prevSong();
    Q_INVOKABLE bool nextSong();
    Q_INVOKABLE bool setSong(quint16 song);

    Q_INVOKABLE void setPlaytime(quint64 ms);

    Q_INVOKABLE bool setAudioSink(CMBaseAudioSink *sink);

    Q_INVOKABLE QSortFilterProxyModel *getSongModel();
    Q_INVOKABLE QVariantMap get(int index);

    Q_INVOKABLE CMMediaScanner *getMediaScanner();

    Q_INVOKABLE bool refreshDatabase(bool force=false);

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

    bool setFilter(const QString filter);
private:
    CMBaseAudioSource *m_source;
    CMBaseAudioSink *m_sink;
    QAudio::State m_state;
    CMMediaDecoder m_dec;
    CMMediaScanner m_scanner;
    quint64 m_playtime;

    QSortFilterProxyModel m_model;

    bool m_playing;
    double m_position;
    double m_length;    
    int m_track;
    int m_tracks;
};

#endif // CMMEDIAPLAYER_H
