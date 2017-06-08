#include "cmqsaaudiosink.h"

#include <QDebug>

CMQSAAudioSink::CMQSAAudioSink(QObject *parent)
    : CMBaseThreadedAudioSink(parent)
    , m_valid(false)
{
    int err;

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

    m_valid=true;
    return;

error:
    qDebug() << "Sound init error: " << snd_strerror(err);
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

bool CMQSAAudioSink::prepareAudio() {
    int err;

    pp.format.interleave = 1;
    pp.format.rate = m_rate;
    pp.format.voices = m_channels;
    pp.format.format = SND_PCM_SFMT_S16_LE;

    if ((err = snd_pcm_plugin_params (handle, &pp)) < 0)
        goto error;

    if ((err = snd_pcm_plugin_prepare (handle, SND_PCM_CHANNEL_PLAYBACK)) < 0)
        goto error;

    return true;

error:
    qDebug() << "Sound error: " << snd_strerror(err);
    return false;
}

void CMQSAAudioSink::setEq(snd_pcm_eq_t eq) {
    int err;

    qDebug() << "EQ Preset " << eq;
    err=snd_pcm_plugin_set_eq(handle, eq);
    if (err<0)
        qDebug() << "EQ Error: " << snd_strerror(err);
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
                qDebug() << "UnderRun Error: " << snd_strerror(err);
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

bool CMQSAAudioSink::isValid()
{
    return m_valid;
}

int CMQSAAudioSink::write(const QByteArray &buffer)
{
    int err, written;
    snd_pcm_channel_status_t status;

    written = snd_pcm_plugin_write(handle, buffer.data(), buffer.size());
    if (written<buffer.size()) {
        qWarning() << "w<l:" << written << " < " << buffer.size();
        err=snd_pcm_plugin_status (handle, &status);
        qDebug() << "Status is: " << status.status;
    }

    return written;
}

void CMQSAAudioSink::prepare()
{
    snd_pcm_plugin_prepare(handle, SND_PCM_CHANNEL_PLAYBACK);
}

void CMQSAAudioSink::drain()
{

}

