#ifndef CMFILEAUDIOSINK_H
#define CMFILEAUDIOSINK_H

#include "cmbaseaudiosink.h"

#include <QFile>
#include <QTimer>

class CMFileAudioSink : public CMBaseAudioSink
{
    Q_OBJECT
public:
    CMFileAudioSink(QObject *parent = 0);

    Q_INVOKABLE bool play();
    Q_INVOKABLE bool stop();
    Q_INVOKABLE bool pause();
    //Q_INVOKABLE bool resume();

    Q_INVOKABLE bool setFile(const QString file);

protected slots:
    void readTicker();

protected:
    virtual bool afterOpen();
    virtual bool aboutToClose();

private:
    QTimer m_timer;
    QFile m_file;
    uint m_readSize;
};

#endif // CMFILEAUDIOSINK_H
