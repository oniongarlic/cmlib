#ifndef CMWAVFILEAUDIOSINK_H
#define CMWAVFILEAUDIOSINK_H

#include <QObject>
#include "cmfileaudiosink.h"

class CMWavFileAudioSink : public CMFileAudioSink
{
    Q_OBJECT
public:
    CMWavFileAudioSink(QObject *parent = 0);

protected:


    // CMFileAudioSink interface
protected:
    bool afterOpen();
    bool aboutToClose();

private:
    int sample_rate;
    int channels;
    int bps;
    int byte_rate;
};

#endif // CMWAVFILEAUDIOSINK_H
