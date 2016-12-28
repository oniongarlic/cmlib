#ifndef CMBASEAUDIOSINK_H
#define CMBASEAUDIOSINK_H

#include <QObject>

class CMBaseAudioSink : public QObject
{
    Q_OBJECT
public:
    explicit CMBaseAudioSink(QObject *parent = 0);
    bool play();
    bool stop();
    bool pause();
    // bool resume();

signals:

public slots:
};

#endif // CMBASEAUDIOSINK_H
