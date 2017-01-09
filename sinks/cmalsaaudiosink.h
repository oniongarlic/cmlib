#ifndef CMALSAAUDIOSINK_H
#define CMALSAAUDIOSINK_H

#include <QObject>

#include <alsa/asoundlib.h>

#include "cmbasethreadedaudiosink.h"

class CMAlsaAudioSink : public CMBaseThreadedAudioSink
{
public:
    CMAlsaAudioSink(QObject *parent = 0);
    ~CMAlsaAudioSink();

private:
    snd_pcm_t *handle;

    // CMBaseThreadedAudioSink interface
protected:
    int write(const QByteArray &buffer);
    void prepare();
    void drain();
};

#endif // CMALSAAUDIOSINK_H
