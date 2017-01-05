#ifndef CMBASETHREADEDAUDIOSINK_H
#define CMBASETHREADEDAUDIOSINK_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

#include "cmbaseaudiosink.h"

class CMPlaybackThread;

class CMBaseThreadedAudioSink : public CMBaseAudioSink
{
    Q_OBJECT
    friend class CMPlaybackThread;
public:
    CMBaseThreadedAudioSink(QObject *parent = 0);
    ~CMBaseThreadedAudioSink();

protected:
    CMPlaybackThread *m_playback;
    QThread *m_pt;
    virtual int write(const QByteArray &buffer);

    // CMBaseAudioSink interface
public:
    bool play();
    bool stop();
    bool pause();

    // CMBaseAudioSink interface
public:
    void setAudioSource(CMBaseAudioSource *source);
};

#endif // CMBASETHREADEDAUDIOSINK_H
