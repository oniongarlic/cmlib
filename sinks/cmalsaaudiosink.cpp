#include "cmalsaaudiosink.h"

#include <QDebug>

#include "cmplaybackthread_p.h"

CMAlsaAudioSink::CMAlsaAudioSink(QObject *parent)
    : CMBaseThreadedAudioSink(parent)
    , handle(0)
{
    int err;

    err=snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (err<0) {
        qDebug() << "PCM open error: " << snd_strerror(err);
        goto init_error;
    }

    if ((err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 2, 44100, 1, 100000)) < 0) {
        qDebug() << "Set params error: " << snd_strerror(err);
        goto init_error;
    }

    return;

    init_error:;
    m_valid=false;
}

CMAlsaAudioSink::~CMAlsaAudioSink()
{
    if (handle) {
        snd_pcm_drain(handle);
        snd_pcm_close(handle);
    }
}

int CMAlsaAudioSink::write(const QByteArray &buffer)
{
    int err=snd_pcm_writei(handle, buffer.data(), buffer.size()/4); // 2-channel, 16-bit
    if (err<0) {
        switch (err) {
        case -EPIPE:
        case -EBADFD:
            snd_pcm_prepare(handle);
            break;
        default:
            qDebug() << "Unhandled ALSA error, trying to recover: " << snd_strerror(err);
            snd_pcm_prepare(handle);
            break;
        }
    }

    return err;
}

void CMAlsaAudioSink::prepare()
{
    int err=snd_pcm_prepare(handle);
    if (err<0)
        qDebug() << "ALSA prepare error: " << snd_strerror(err);
}

void CMAlsaAudioSink::drain()
{
    int err=snd_pcm_drain(handle);
    if (err<0)
        qDebug() << "ALSA drain error: " << snd_strerror(err);
}
