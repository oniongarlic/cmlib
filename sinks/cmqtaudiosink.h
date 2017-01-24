#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <math.h>

#include <QObject>
#include <QIODevice>
#include <QTimer>
#include <QByteArray>
#include <QStringList>

#include <QAudioOutput>

#include "cmbaseaudiosink.h"
#include "cmbaseaudiosource.h"

class CMQtAudioSink : public CMBaseAudioSink
{
    Q_OBJECT
public:
    explicit CMQtAudioSink(QObject *parent = 0);
    ~CMQtAudioSink();

    Q_INVOKABLE bool play();
    Q_INVOKABLE bool stop();
    Q_INVOKABLE bool pause();
    Q_INVOKABLE bool resume();

private:
    void initializeAudio();
    void createAudioOutput();

private:    
    QAudioDeviceInfo m_device;    
    QAudioOutput*    m_audioOutput;
    QIODevice*       m_output; // not owned
    QAudioFormat     m_format;    
    QByteArray       m_buffer;

private slots:
    void notified();    
    void toggleSuspendResume();
    void stateChanged(QAudio::State state);
    void deviceChanged(int index);
};

#endif

