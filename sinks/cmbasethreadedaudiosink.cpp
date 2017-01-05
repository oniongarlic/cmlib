#include "cmbasethreadedaudiosink.h"

#include "cmplaybackthread_p.h"

#include <QDebug>

CMPlaybackThread::CMPlaybackThread(QObject *parent)
    : QObject(parent)
    , isRunning(true)
    , isPlaying(false)
{

}

void CMPlaybackThread::playLoop()
{
    QThread *cur;

    cur=QThread::currentThread();
    qDebug() << "Thread is running :" << cur->currentThreadId();

    while (isRunning) {
        sync.lock();
        if (!isPlaying) {
            qDebug("Waiting...");
            pauseCond.wait(&sync);
            // prepareAudio();
            qDebug("...Resuming");
        }

        // checkStatus();

        if (isPlaying) {
            qDebug(".");
        }
        sync.unlock();
    }
    qDebug("playLoop is going away");
}

CMBaseThreadedAudioSink::CMBaseThreadedAudioSink(QObject *parent)
    : CMBaseAudioSink(parent)
{
    m_pt=new QThread();
    m_playback=new CMPlaybackThread();
    m_playback->moveToThread(m_pt);

    connect(m_pt, SIGNAL(started()), m_playback, SLOT(playLoop()));
    connect(m_pt, SIGNAL(finished()), m_playback, SLOT(deleteLater()));

    //connect(m_playback, SIGNAL(playingChanged(bool)), this, SLOT(onPlayingChanged()));
    //connect(m_playback, SIGNAL(positionChanged(int)), this, SLOT(onPositionChanged(int)));

    m_pt->start();
}

bool CMBaseThreadedAudioSink::play()
{
    m_playback->sync.lock();
    m_playback->isPlaying=true;
    m_playback->sync.unlock();
}

bool CMBaseThreadedAudioSink::stop()
{
    m_playback->sync.lock();
    m_playback->isPlaying=false;
    m_playback->sync.unlock();
}

bool CMBaseThreadedAudioSink::pause()
{
    m_playback->sync.lock();
    m_playback->isPlaying=false;
    m_playback->sync.unlock();
}
