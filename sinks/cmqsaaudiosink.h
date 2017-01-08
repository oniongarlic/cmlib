#ifndef CMQSAAUDIOSINK_H
#define CMQSAAUDIOSINK_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

#include "cmbasethreadedaudiosink.h"

#include <sys/asound.h>
#include <sys/asoundlib.h>
#include <audio/audio_manager_routing.h>

class CMQSAAudioSink : public CMBaseThreadedAudioSink
{
public:
    explicit CMQSAAudioSink(QObject *parent = 0);
    ~CMQSAAudioSink();

    Q_INVOKABLE void setEq(snd_pcm_eq_t eq);

private:
    unsigned int am_handle;
    snd_pcm_t *handle;
    snd_pcm_info_t pi;
    snd_pcm_channel_info_t pci;
    snd_pcm_channel_params_t pp;
    snd_pcm_channel_setup_t setup;

    bool prepareAudio();
    void checkStatus();

    bool m_isPlaying;
    bool m_valid;
    bool m_isRunning;

    // CMBaseAudioSink interface
public:
    bool isValid();

    // CMBaseThreadedAudioSink interface
protected:
    int write(const QByteArray &buffer);
    void prepare();
    void drain();
};

#endif // CMQSAAUDIOSINK_H
