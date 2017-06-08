#ifndef CMBASEAUDIOSINK_H
#define CMBASEAUDIOSINK_H

#include <QObject>
#include <QAudio>

#include "cmbaseaudiosource.h"

class CMBaseAudioSink : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAudio::State state READ state NOTIFY stateChanged)
    Q_PROPERTY(uint rate READ rate NOTIFY rateChanged)
    Q_PROPERTY(uint channels READ channels NOTIFY channelsChanged)
public:
    explicit CMBaseAudioSink(QObject *parent = 0);

    virtual bool play()=0;
    virtual bool stop()=0;
    virtual bool pause()=0;

    virtual bool isValid();

    virtual void setAudioSource(CMBaseAudioSource *source);

    QAudio::State state() const;

    uint channels() const;

    uint rate() const;

signals:
    void stateChanged(QAudio::State state);
    void rateChanged(uint rate);
    void channelsChanged(uint channels);
    void position(qint64 pos);

public slots:

protected:
    void setState(QAudio::State state);
    CMBaseAudioSource *m_source;
    QAudio::State m_state;
    bool m_valid;
    uint m_channels;
    uint m_rate;
};

#endif // CMBASEAUDIOSINK_H
