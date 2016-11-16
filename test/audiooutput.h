#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <math.h>

#include <QObject>
#include <QIODevice>
#include <QTimer>
#include <QByteArray>
#include <QAudioOutput>

#include "cmsidaudiosource.h"
#include "cmmodplugaudiosource.h"
#include "cmsapaudiosource.h"

class AudioTest : public QObject
{
    Q_OBJECT
public:
    AudioTest();
    ~AudioTest();

private:
    void initializeAudio();
    void createAudioOutput();

private:
    QTimer*          m_pullTimer;

    QAudioDeviceInfo m_device;
    CMBaseAudioSource*  m_generator;
    QAudioOutput*    m_audioOutput;
    QIODevice*       m_output; // not owned
    QAudioFormat     m_format;

    bool             m_pullMode;
    QByteArray       m_buffer;


private slots:
    void notified();
    void pullTimerExpired();
    void toggleMode();
    void toggleSuspendResume();
    void stateChanged(QAudio::State state);
    void deviceChanged(int index);
};

#endif

