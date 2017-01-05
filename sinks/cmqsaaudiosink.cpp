#include "cmqsaaudiosink.h"

CMQSAAudioSink::CMQSAAudioSink(QObject *parent)
    : CMBaseAudioSink(parent)
{
    if ((err = audio_manager_snd_pcm_open_preferred(AUDIO_TYPE_MULTIMEDIA, &handle, &am_handle, NULL, NULL, SND_PCM_OPEN_PLAYBACK)) < 0)
        goto error;
    snd_pcm_plugin_set_enable(handle, PLUGIN_ROUTING | PLUGIN_EQ);

    memset (&pi, 0, sizeof (pi));
    if ((err = snd_pcm_info (handle, &pi)) < 0)
        goto error;

    memset (&pci, 0, sizeof (pci));
    pci.channel = SND_PCM_CHANNEL_PLAYBACK;

    if ((err = snd_pcm_plugin_info (handle, &pci)) < 0)
        goto error;

    memset (&pp, 0, sizeof (pp));
    pp.mode = SND_PCM_MODE_BLOCK;
    pp.channel = SND_PCM_CHANNEL_PLAYBACK;
    pp.start_mode = SND_PCM_START_FULL;
    pp.stop_mode = SND_PCM_STOP_STOP;

    pp.buf.block.frag_size = pci.max_fragment_size;
    pp.buf.block.frags_max = 16; // XXX: What should this be?
    pp.buf.block.frags_min = 8;

    if (!prepareAudio())
        goto error;

    return true;

error:
    qDebug() << "Sound error: " << snd_strerror(err);
    if (handle) {
        snd_pcm_close(handle);
        handle=0;
    }
}

CMQSAAudioSink::~CMQSAAudioSink()
{
    if (handle) {
        snd_pcm_close(handle);
        audio_manager_free_handle(am_handle);
    }
}

bool CMQSAAudioSink::play()
{

}

bool CMQSAAudioSink::stop()
{

}

bool CMQSAAudioSink::pause()
{

}

void CMQSAAudioSink::playLoop() {
    QThread *cur;
    int err;
    size_t written=0;
    int pos=0,track=0;
    size_t length=pci.max_fragment_size;
    snd_pcm_channel_status_t status;

    cur=QThread::currentThread();

    memset (&status, 0, sizeof (status));
    status.channel = SND_PCM_CHANNEL_PLAYBACK;

    qDebug() << "Thread is running :" << cur->currentThreadId();

    while (m_isRunning) {
        int r;

        sync.lock();
        if (!m_isPlaying) {
            qDebug("Waiting...");
            pauseCond.wait(&sync);
            prepareAudio();
            qDebug("...Resuming");
        }

        checkStatus();

        if (m_isPlaying) {
            // XXX


            written = snd_pcm_plugin_write(handle, m_buffer.data(), length);
            if (written<length) {
                qWarning() << "w<l:" << written << " < " << length;
                err=snd_pcm_plugin_status (handle, &status);
                qDebug() << "Status is: " << status.status;
            }
        }
        sync.unlock();
        if (m_pos!=pos) {
            m_pos=pos;
            emit positionChanged(m_pos);
        }
        if (m_current!=track && track>0) {
            m_current=track;
            track=0;
            emit trackChanged(m_current);
        }
    }
    qDebug("playLoop is going away");
}

bool CMQSAAudioSink::prepareAudio() {
    int err;

    pp.format.interleave = 1;
    pp.format.rate = m_freq;
    pp.format.voices = m_channels;
    pp.format.format = SND_PCM_SFMT_S16_LE;

    if ((err = snd_pcm_plugin_params (handle, &pp)) < 0)
        goto error;

    if ((err = snd_pcm_plugin_prepare (handle, SND_PCM_CHANNEL_PLAYBACK)) < 0)
        goto error;

    qDebug() << "Rate is " << pp.format.rate;

    qDebug() << "Plugin state " << snd_pcm_plugin_get_inactive(handle);

    return true;

error:
    qDebug() << "Sound error: " << snd_strerror(err);
    return false;
}

void CMQSAAudioSink::setEq(snd_pcm_eq_t eq) {
    int err;

    qDebug() << "EQ Preset " << eq;
    sync.lock();
    err=snd_pcm_plugin_set_eq(handle, eq);
    if (err<0)
        qDebug() << "EQ Error: " << snd_strerror(err);
    sync.unlock();
}

void CMQSAAudioSink::checkStatus() {
    int err;
    snd_pcm_channel_status_t status;

    err=snd_pcm_plugin_status (handle, &status);
    if (err==0) {
        switch (status.status) {
        case SND_PCM_STATUS_NOTREADY:
            break;
        case SND_PCM_STATUS_UNDERRUN:
            qDebug("Resuming playback channel");
            err = snd_pcm_plugin_prepare(handle, SND_PCM_CHANNEL_PLAYBACK);
            if (err<0) {
                m_isPlaying=false;
                emit playingChanged(m_isPlaying);
                emit error();
            }
            break;
        case SND_PCM_STATUS_CHANGE:
            qDebug("Stream change");
            prepareAudio();
            break;
        case SND_PCM_STATUS_READY:
        case SND_PCM_STATUS_PREPARED:
        case SND_PCM_STATUS_RUNNING:
            // All OK, do nothing
            break;
        default:
            qWarning() << "Unhandled status " << status.status;
        }
    }
}

