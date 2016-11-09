#ifndef CMMODPLUGAUDIOSOURCE_H
#define CMMODPLUGAUDIOSOURCE_H

#include <QObject>
#include <libmodplug/modplug.h>

#include "cmbaseaudiosource.h"

class CMModPlugAudioSource : public CMBaseAudioSource
{
    Q_OBJECT
public:
    CMModPlugAudioSource(QObject *parent);

    void close();
    bool reset();
    bool open(QIODevice::OpenMode mode);
protected:
    bool generateData(qint64 maxlen);
private:
    ModPlugFile *m_modplug;
    ModPlug_Settings settings;
    QByteArray m_tune;
    qint64 writeData(const char *data, qint64 len);
};

#endif // CMMODPLUGAUDIOSOURCE_H
