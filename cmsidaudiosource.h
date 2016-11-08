#ifndef CMSIDAUDIOSOURCE_H
#define CMSIDAUDIOSOURCE_H

#include "cmbaseaudiosource.h"

#include <sidplayfp/sidplayfp.h>
#include <sidplayfp/event.h>
#include <sidplayfp/SidTune.h>
#include <sidplayfp/SidInfo.h>
#include <sidplayfp/SidTuneInfo.h>
#include <sidplayfp/sidbuilder.h>
#include <sidplayfp/builders/residfp.h>
#include <sidplayfp/builders/resid.h>

class CMSidAudioSource : public CMBaseAudioSource
{
    Q_OBJECT
public:
    explicit CMSidAudioSource(QObject *parent);
    ~CMSidAudioSource();
    bool open(OpenMode mode);
    void close();
    qint64 pos() const;
    bool reset();

    // CMBaseAudioSource interface
public slots:
    void setTrack(quint16 track);

private:
    sidplayfp     *engine;
    SidTune       *tune;
    SidConfig     config;
    ReSIDfpBuilder *rs;
    QByteArray m_tune;

    bool prepareTune();

protected:
    bool generateData(qint64 maxlen);   
    qint64 writeData(const char *data, qint64 len);

};

#endif // CMSIDAUDIOSOURCE_H
