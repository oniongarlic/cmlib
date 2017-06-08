#ifndef CMOPENMTPAUDIOSOURCE_H
#define CMOPENMTPAUDIOSOURCE_H

#include <QObject>

#include "cmbaseaudiosource.h"

#include <libopenmpt/libopenmpt.hpp>

class CMOpenMTPAudioSource : public CMBaseAudioSource
{
    Q_OBJECT
    Q_CLASSINFO("mime", "audio/x-mod")
    Q_CLASSINFO("extensions", "*.mod;*.xm")
public:
    CMOpenMTPAudioSource(QObject *parent);

    void close();
    bool reset();
    bool open(QIODevice::OpenMode mode);

    static QStringList extensions();

protected:
    qint64 generateData(qint64 maxlen);
private:
    openmpt::module *mod;

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

    // CMBaseAudioSource interface
public slots:
    void setTrack(quint16 track);
};

#endif // CMOPENMTPAUDIOSOURCE_H
