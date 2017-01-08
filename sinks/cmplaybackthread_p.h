#ifndef CMPLAYBACKTHREAD_P_H
#define CMPLAYBACKTHREAD_P_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

#include "cmbaseaudiosource.h"
#include "cmbasethreadedaudiosink.h"

class CMPlaybackThread : public QObject
{
    Q_OBJECT

public:
    CMPlaybackThread(QObject *parent=0);

    QMutex sync;
    QWaitCondition pauseCond;

    bool isRunning;
    bool isPlaying;

    CMBaseAudioSource *source;
    CMBaseThreadedAudioSink *sink;

    bool play();
    bool stop();
    void exit();

public slots:
    void playLoop();

protected:
    size_t frag_size;
    QByteArray m_buffer;
};

#endif // CMPLAYBACKTHREAD_P_H
