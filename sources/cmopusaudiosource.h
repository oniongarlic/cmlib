#ifndef CMOPUSAUDIOSOURCE_H
#define CMOPUSAUDIOSOURCE_H

#include <QObject>
#include <QDebug>

#include <opusfile.h>

#include "cmbaseaudiosource.h"

class CMOpusAudioSource : public CMBaseAudioSource
{
    Q_OBJECT
    Q_CLASSINFO("mime", "audio/x-opus")
    Q_CLASSINFO("extensions", "*.opus")
public:    
    explicit CMOpusAudioSource(QObject *parent=0);
    ~CMOpusAudioSource();    

    static QStringList extensions();

    bool open(OpenMode mode);
    void close();
    bool reset();

protected:
    qint64 generateData(qint64 maxlen);

private:
    OggOpusFile *m_opus;
    float m_track_pos;    
};

#endif // CMOPUSAUDIOSOURCE_H
