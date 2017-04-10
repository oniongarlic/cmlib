#include "cmbasethreadedaudiosink.h"

#include "cmplaybackthread_p.h"

#include <QDebug>

CMPlaybackThread::CMPlaybackThread(QObject *parent)
    : QObject(parent)
    , isRunning(true)
    , isPlaying(false)
    , frag_size(4096)
    , m_buffer(65535, 0)
{

}

bool CMPlaybackThread::play()
{
    bool r=true;

    sync.lock();
    isPlaying=true;
    sync.unlock();
    pauseCond.wakeAll();

    return r;
}

bool CMPlaybackThread::stop()
{
    bool r=true;

    sync.lock();
    isPlaying=false;
    if (sink)
        sink->drain();
    sync.unlock();
    pauseCond.wakeAll();    

    return r;
}

void CMPlaybackThread::exit()
{
    sync.lock();
    isPlaying=false;
    if (sink)
        sink->drain();
    isRunning=false;
    sync.unlock();
    pauseCond.wakeAll();
}

void CMPlaybackThread::playLoop()
{
    QThread *cur=QThread::currentThread();

    m_buffer.resize(frag_size);

    qDebug() << "Thread is running :" << cur->currentThreadId();
    qDebug() << "Buffer size is " << m_buffer.size();

    while (isRunning) {
        sync.lock();
        if (!isPlaying) {
            qDebug("Waiting...");
            pauseCond.wait(&sync);
            if (sink)
                sink->prepare();
            qDebug("...Resuming");
            if (!source) {
                qWarning("Source is not set !");
                isPlaying=false;
            }
        }

        // checkStatus();

        if (isPlaying) {
            Q_ASSERT(source);
            Q_ASSERT(sink);
            sync.unlock();
            int r=source->read(m_buffer.data(), frag_size);
            sync.lock();
            if (r>0)
                sink->write(m_buffer);
        }
        sync.unlock();
    }
    qDebug("playLoop is going away");
    cur->exit();
}

CMBaseThreadedAudioSink::CMBaseThreadedAudioSink(QObject *parent)
    : CMBaseAudioSink(parent)
{
    m_pt=new QThread(this);
    m_playback=new CMPlaybackThread();
    m_playback->sink=this;
    m_playback->moveToThread(m_pt);

    connect(m_pt, SIGNAL(started()), m_playback, SLOT(playLoop()));
    connect(m_pt, SIGNAL(finished()), m_playback, SLOT(deleteLater()));

    //connect(m_playback, SIGNAL(playingChanged(bool)), this, SLOT(onPlayingChanged()));
    //connect(m_playback, SIGNAL(positionChanged(int)), this, SLOT(onPositionChanged(int)));

    m_pt->start();
}

CMBaseThreadedAudioSink::~CMBaseThreadedAudioSink()
{
    m_playback->exit();
    m_pt->wait();
}

int CMBaseThreadedAudioSink::write(const QByteArray &buffer)
{
    qDebug() << "Buffer: " << buffer.size();
    return 0;
}

void CMBaseThreadedAudioSink::prepare()
{

}

void CMBaseThreadedAudioSink::drain()
{

}

bool CMBaseThreadedAudioSink::play()
{
    bool r;

    if (!m_source->isOpen()) {
        qDebug("Opening source");
        r=m_source->open(QIODevice::ReadOnly);
        if (!r) {
            qWarning("Open for playback failed");
            return false;
        }
    }
    m_playback->play();
    setState(QAudio::ActiveState);

    return true;
}

bool CMBaseThreadedAudioSink::stop()
{
    m_playback->stop();
    setState(QAudio::StoppedState);

    return true;
}

bool CMBaseThreadedAudioSink::pause()
{
    m_playback->stop();
    setState(QAudio::IdleState);

    return true;
}

void CMBaseThreadedAudioSink::setAudioSource(CMBaseAudioSource *source)
{
    qDebug("Setting thread source");
    m_playback->sync.lock();
    m_source=source;
    m_playback->source=m_source;
    m_playback->sync.unlock();
    //m_playback->pauseCond.wakeAll();
}
