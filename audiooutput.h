#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <math.h>

#include <QObject>
#include <QIODevice>
#include <QTimer>
#include <QByteArray>
#include <QAudioOutput>

#include <sidplayfp/sidplayfp.h>
#include <sidplayfp/event.h>
#include <sidplayfp/SidTune.h>
#include <sidplayfp/SidInfo.h>
#include <sidplayfp/SidTuneInfo.h>
#include <sidplayfp/sidbuilder.h>
#include <sidplayfp/builders/residfp.h>
#include <sidplayfp/builders/resid.h>

class Generator : public QIODevice
{
    Q_OBJECT
public:
    Generator(QObject *parent);
    ~Generator();

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;

private:
    void generateData(qint64 len);

private:
    qint64 m_pos;
    QByteArray m_buffer;

    sidplayfp     *engine;
    SidTune       *tune;
    SidConfig     config;
    ReSIDfpBuilder *rs;
};

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
    Generator*       m_generator;
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

