#ifndef CMSIDAUDIOSOURCE_H
#define CMSIDAUDIOSOURCE_H

#include "cmbaseaudiosource.h"

#ifdef Q_OS_DARWIN
// XXX: Workaround C++, LLVM, whatever, don't care
#ifndef HAVE_CXX11
#  define auto_ptr unique_ptr
#endif
#endif

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
    Q_CLASSINFO("mime", "audio/x-sid")
    Q_CLASSINFO("extensions", "*.sid")
public:
    explicit CMSidAudioSource(QObject *parent);
    ~CMSidAudioSource();
    bool open(OpenMode mode);
    void close();   
    bool reset();

    // CMBaseAudioSource interface
public slots:
    void setTrack(quint16 track);

private:
    Q_DISABLE_COPY(CMSidAudioSource)
    sidplayfp     *engine;
    SidTune       *tune;
    SidConfig     config;
    ReSIDfpBuilder *rs;

    bool prepareTune();

protected:
    qint64 generateData(qint64 maxlen);
};

#endif // CMSIDAUDIOSOURCE_H
