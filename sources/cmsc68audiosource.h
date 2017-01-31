#ifndef CMSC68AUDIOSOURCE_H
#define CMSC68AUDIOSOURCE_H

#include <QObject>

#include "cmbaseaudiosource.h"

#include <sc68/sc68.h>

class CMSC68AudioSource : public CMBaseAudioSource
{
    Q_OBJECT
    Q_CLASSINFO("mime", "audio/x-sc68")
public:
    explicit CMSC68AudioSource(QObject *parent=NULL);
    ~CMSC68AudioSource();

    // CMBaseAudioSource interface
    bool open(OpenMode mode);
    void close();
    bool reset();

    static QStringList extensions();

    static bool initOnce;

public slots:
    void setTrack(quint16 track);

protected:
    qint64 generateData(qint64 maxlen);

private:
    sc68_t *m_sc68;
};

#endif // CMSC68AUDIOSOURCE_H
