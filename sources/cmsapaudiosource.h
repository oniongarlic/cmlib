#ifndef CMSAPAUDIOSOURCE_H
#define CMSAPAUDIOSOURCE_H

#include "cmbaseaudiosource.h"
#include <QObject>
#include <asap.h>

class CMSapAudioSource : public CMBaseAudioSource
{
    Q_OBJECT
    Q_CLASSINFO("mime", "audio/x-sap")
    Q_CLASSINFO("extensions", "*.sap;*.rmt;*.cmc;*.cm3;*.cmr;*.cms;*.dmc;*.dlt;*.mpt;*.mpd;*.tmc;*.tm8;*.tm2;*.fc")
public:
    explicit CMSapAudioSource(QObject *parent);
    ~CMSapAudioSource();

    bool open(QIODevice::OpenMode mode);
    void close();
    bool reset();

    static QStringList extensions();

public slots:
    void setTrack(quint16 track);
protected:
    qint64 generateData(qint64 maxlen);
private:
    ASAP *m_asap;
    const ASAPInfo *m_info;
    int m_song_channels;
};

#endif // CMSAPAUDIOSOURCE_H
