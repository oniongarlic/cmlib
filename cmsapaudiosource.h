#ifndef CMSAPAUDIOSOURCE_H
#define CMSAPAUDIOSOURCE_H

#include "cmbaseaudiosource.h"
#include <QObject>
#include <asap.h>

class CMSapAudioSource : public CMBaseAudioSource
{
    Q_OBJECT
public:
    explicit CMSapAudioSource(QObject *parent);

    bool open(QIODevice::OpenMode mode);
    void close();
public slots:
    void setTrack(quint16 track);
protected:
    bool generateData(qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
private:
    QByteArray m_tune;
    ASAP *m_asap;
    const ASAPInfo *m_info;
};

#endif // CMSAPAUDIOSOURCE_H
