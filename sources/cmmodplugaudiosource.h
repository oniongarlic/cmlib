#ifndef CMMODPLUGAUDIOSOURCE_H
#define CMMODPLUGAUDIOSOURCE_H

#include <QObject>
#include <libmodplug/modplug.h>

#include "cmbaseaudiosource.h"

class CMModPlugAudioSource : public CMBaseAudioSource
{
    Q_OBJECT
    Q_CLASSINFO("mime", "audio/x-mod")
    Q_CLASSINFO("extensions", "*.mod;*.xm;*.it;*.s3m;*.stm;*.669;*.amf;*.ams;*.dbm;*.dmf;*.dsm;*.far;*.mdl;*.med;*.mtm;*.okt;*.ptm;*.ult;*.umx;*.mt2;*.psm")
public:
    CMModPlugAudioSource(QObject *parent);

    void close();
    bool reset();
    bool open(QIODevice::OpenMode mode);

protected:
    qint64 generateData(qint64 maxlen);
private:
    ModPlugFile *m_modplug;
    ModPlug_Settings settings;
    QByteArray m_tune;
    qint64 writeData(const char *data, qint64 len);
    quint64 m_track_pos;

    // Mod specific extra data
    qint32 m_speed;
    qint32 m_tempo;
    qint32 m_order;
    qint32 m_pattern;
    qint32 m_row;
    qint32 m_pchannels;
};

#endif // CMMODPLUGAUDIOSOURCE_H
