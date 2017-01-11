#ifndef CMSC68AUDIOSOURCE_H
#define CMSC68AUDIOSOURCE_H

#include <QObject>

#include "cmbaseaudiosource.h"

#include <sc68/sc68.h>

class CMSC68AudioSource : public CMBaseAudioSource
{
    Q_OBJECT
public:
    explicit CMSC68AudioSource(QObject *parent=NULL);

    // CMBaseAudioSource interface

    static QStringList extensions();

protected:
    bool generateData(qint64 maxlen);

private:
    sc68_t *m_sc68;
};

#endif // CMSC68AUDIOSOURCE_H
