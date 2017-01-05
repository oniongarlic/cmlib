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
public:
    CMBaseThreadedAudioSink(QObject *parent = 0);

protected:
    CMPlaybackThread *m_playback;
    QThread *m_pt;

    // CMBaseAudioSink interface
public:
    bool play();
    bool stop();
    bool pause();
};

#endif // CMBASETHREADEDAUDIOSINK_H
