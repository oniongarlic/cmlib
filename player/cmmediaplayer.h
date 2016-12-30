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
public:
    explicit CMMediaPlayer(QObject *parent = 0);

    Q_INVOKABLE bool load(const QString file);
    bool load(const QByteArray &data, CMBaseAudioSource *source);
    Q_INVOKABLE bool play();
    Q_INVOKABLE bool stop();
    Q_INVOKABLE bool pause();

    bool setAudioSink(CMBaseAudioSink *sink);

    QAudio::State getState();

signals:    
    void metadata(QVariantHash meta);

protected slots:
    void decoderMetadata(QVariantHash meta);

public slots:

private:
    CMBaseAudioSource *m_source;
    CMBaseAudioSink *m_sink;
    QAudio::State m_state;
    CMMediaDecoder m_dec;
};

#endif // CMMEDIAPLAYER_H
