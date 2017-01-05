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
            // prepareAudio();
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
            qDebug("R");
            source->read(m_buffer.data(), frag_size);
            qDebug("W");
            sink->write(m_buffer);
        }
        sync.unlock();
    }
    qDebug("playLoop is going away");
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
    m_playback->sync.lock();
    m_playback->isPlaying=false;
    m_playback->isRunning=false;
    m_playback->sync.unlock();
    m_playback->pauseCond.wakeAll();
}

int CMBaseThreadedAudioSink::write(const QByteArray &buffer)
{
    qDebug() << "Buffer: " << buffer.size();
    return 0;
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
    m_playback->sync.lock();
    m_playback->isPlaying=true;
    m_playback->sync.unlock();
    m_playback->pauseCond.wakeAll();

    return true;
}

bool CMBaseThreadedAudioSink::stop()
{
    m_playback->sync.lock();
    m_playback->isPlaying=false;
    m_playback->sync.unlock();
    m_playback->pauseCond.wakeAll();

    return true;
}

bool CMBaseThreadedAudioSink::pause()
{
    m_playback->sync.lock();
    m_playback->isPlaying=false;
    m_playback->sync.unlock();
    m_playback->pauseCond.wakeAll();

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
