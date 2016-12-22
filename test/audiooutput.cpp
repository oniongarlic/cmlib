#include <QDebug>
#include <QFile>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QtCore/qmath.h>
#include <QtCore/qendian.h>

#include "audiooutput.h"

const int DataFrequencyHz = 44100;
const int BufferSize      = 65535;

AudioTest::AudioTest()
    :   m_pullTimer(new QTimer(this))
    ,   m_device(QAudioDeviceInfo::defaultOutputDevice())
    ,   m_generator(0)
    ,   m_audioOutput(0)
    ,   m_output(0)
    ,   m_buffer(BufferSize, 0)
{
    qDebug("InitAudio");

    initializeAudio();
}

void AudioTest::initializeAudio()
{
    connect(m_pullTimer, SIGNAL(timeout()), SLOT(pullTimerExpired()));

    m_pullMode = false;

    m_format.setFrequency(DataFrequencyHz);
    m_format.setChannels(2);
    m_format.setSampleSize(16);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);

    qDebug() << m_device.deviceName();
    if (!m_device.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = m_device.nearestFormat(m_format);
    }   

    createAudioOutput();
}

void AudioTest::createAudioOutput()
{
    m_audioOutput = new QAudioOutput(m_device, m_format, this);
    connect(m_audioOutput, SIGNAL(notify()), SLOT(notified()));
    connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));    
    m_audioOutput->setBufferSize(165535*4);  

    qDebug() << "Period size is: " << m_audioOutput->periodSize();

    //m_output = m_audioOutput->start();
    //m_pullMode = false;
    //m_pullTimer->start(100);
}

AudioTest::~AudioTest()
{

}

bool AudioTest::play()
{
    bool r=m_generator->open(QIODevice::ReadOnly);
    if (r)
        m_audioOutput->start(m_generator);
    else
        qWarning("Open for playback failed");

    qDebug() << "Buffer size used is: " << m_audioOutput->bufferSize();

    return r;
}

bool AudioTest::stop()
{
    m_audioOutput->stop();

    return true;
}

bool AudioTest::pause()
{
    m_audioOutput->suspend();

    return true;
}

bool AudioTest::resume()
{
    m_audioOutput->resume();

    return true;
}

bool AudioTest::load(QString file, CMBaseAudioSource *decoder)
{
    QFile sf(file);
    sf.open(QIODevice::ReadOnly);
    QByteArray sdata=sf.readAll();

    m_generator=decoder;
    m_generator->open(QIODevice::WriteOnly);
    m_generator->write(sdata.constData(), sdata.size());
    m_generator->close();

    return true;
}

void AudioTest::deviceChanged(int index)
{
    m_pullTimer->stop();
    m_generator->close();
    m_audioOutput->stop();
    m_audioOutput->disconnect(this);   
    createAudioOutput();
}

void AudioTest::notified()
{
    qWarning() << "bytesFree = " << m_audioOutput->bytesFree()
               << ", " << "elapsedUSecs = " << m_audioOutput->elapsedUSecs()
               << ", " << "processedUSecs = " << m_audioOutput->processedUSecs();
}

void AudioTest::pullTimerExpired()
{
    if (m_audioOutput && m_audioOutput->state() != QAudio::StoppedState) {
        int chunks = m_audioOutput->bytesFree()/m_audioOutput->periodSize();
        while (chunks) {
           const qint64 len = m_generator->read(m_buffer.data(), m_audioOutput->periodSize());
           if (len)
               m_output->write(m_buffer.data(), len);
           if (len != m_audioOutput->periodSize())
               break;
           --chunks;
        }
    }
}

void AudioTest::toggleMode()
{
    m_pullTimer->stop();
    m_audioOutput->stop();

    if (m_pullMode) {
        m_output = m_audioOutput->start();
        m_pullMode = false;
        m_pullTimer->start(20);
    } else {
        m_pullMode = true;
        m_audioOutput->start(m_generator);
    }

}

void AudioTest::toggleSuspendResume()
{
    if (m_audioOutput->state() == QAudio::SuspendedState) {
        qWarning() << "status: Suspended, resume()";
        m_audioOutput->resume();
    } else if (m_audioOutput->state() == QAudio::ActiveState) {
        qWarning() << "status: Active, suspend()";
        m_audioOutput->suspend();
    } else if (m_audioOutput->state() == QAudio::StoppedState) {
        qWarning() << "status: Stopped, resume()";
        m_audioOutput->resume();
    } else if (m_audioOutput->state() == QAudio::IdleState) {
        qWarning() << "status: IdleState";
    }
}

void AudioTest::stateChanged(QAudio::State state)
{
    qWarning() << "state = " << state;
}