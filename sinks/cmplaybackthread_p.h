#ifndef CMPLAYBACKTHREAD_P_H
#define CMPLAYBACKTHREAD_P_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

#include "cmbaseaudiosource.h"

class CMPlaybackThread : public QObject
{
    Q_OBJECT

public:
    CMPlaybackThread(QObject *parent=0);
    void playLoop();

    QMutex sync;
    QWaitCondition pauseCond;

    bool isPlaying;
    bool isRunning;

    CMBaseAudioSource *source;
};

#endif // CMPLAYBACKTHREAD_P_H
