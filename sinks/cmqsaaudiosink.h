#ifndef CMQSAAUDIOSINK_H
#define CMQSAAUDIOSINK_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

#include "cmbaseaudiosink.h"

#include <sys/asound.h>
#include <sys/asoundlib.h>
#include <audio/audio_manager_routing.h>

class CMQSAAudioSink : public CMBaseAudioSink
{
public:
    explicit CMQSAAudioSink(QObject *parent = 0);
    ~CMQSAAudioSink();

    bool play();
    bool stop();
    bool pause();

    Q_INVOKABLE void setEq(snd_pcm_eq_t eq);

private:
    unsigned int am_handle;
    snd_pcm_t *handle;
    snd_pcm_info_t pi;
    snd_pcm_channel_info_t pci;
    snd_pcm_channel_params_t pp;
    snd_pcm_channel_setup_t setup;

    QByteArray m_buffer;

    bool prepareAudio();
    void checkStatus();

    QMutex sync;
    QWaitCondition pauseCond;

    bool m_isPlaying;
    bool m_valid;
    bool m_isRunning;

    // CMBaseAudioSink interface
public:
    bool isValid();
};

#endif // CMQSAAUDIOSINK_H
