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
public:
    explicit CMMediaPlayer(QObject *parent = 0);

    Q_INVOKABLE bool load(const QString file);
    bool load(const QByteArray &data, CMBaseAudioSource *source);
    Q_INVOKABLE bool play();
    Q_INVOKABLE bool stop();
    Q_INVOKABLE bool pause();
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

signals:
    void metadata(QVariantHash meta);
    void positionChanged(quint64 position);
    void lengthChanged(double length);

protected slots:
    void decoderMetadata(QVariantHash meta);
    void sinkPosition(quint64 pos);

public slots:

private:    
    CMBaseAudioSource *m_source;
    CMBaseAudioSink *m_sink;
    QAudio::State m_state;
    CMMediaDecoder m_dec;
    double m_position;
    double m_length;
    quint64 m_playtime;
};

#endif // CMMEDIAPLAYER_H
