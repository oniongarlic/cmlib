#include <QDebug>
#include <QFile>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QtCore/qmath.h>
#include <QtCore/qendian.h>

#include "cmqtaudiosink.h"

const int BufferSize      = 65535;

CMQtAudioSink::CMQtAudioSink(QObject *parent)
    :   CMBaseAudioSink(parent)
    ,   m_device(QMediaDevices::defaultAudioOutput())
    ,   m_audioOutput(0)
    ,   m_output(0)
    ,   m_buffer(BufferSize, 0)
{
    initializeAudio();
}

void CMQtAudioSink::initializeAudio()
{
    m_format.setSampleRate(m_rate);
    m_format.setChannelCount(m_channels);         
    m_format.setSampleFormat(QAudioFormat::Int16);

    if (!m_device.isFormatSupported(m_format)) {
        // XXX
        qWarning() << "Default format not supported - trying to use nearest";
        //m_format = m_device.nearestFormat(m_format);
    }

    createAudioOutput();
}

void CMQtAudioSink::createAudioOutput()
{
    m_sink=new QAudioSink(m_format, this);
    m_sink->setBufferSize(65535);
    connect(m_sink, SIGNAL(notify()), SLOT(notified()));
    connect(m_sink, SIGNAL(stateChanged(QAudio::State)), SLOT(outputStateChanged(QAudio::State)));
}

CMQtAudioSink::~CMQtAudioSink()
{

}

bool CMQtAudioSink::play()
{    
    if (!m_audioOutput) {
        qWarning("AudioOutput not set!");
        return false;
    }

    if (!m_source) {
        qWarning("Source not set!");
        return false;
    }

    QAudio::State s=m_sink->state();

    qDebug() << "Play: State is " << s;

    switch (s) {
    case QAudio::ActiveState:
        //
        break;
    case QAudio::SuspendedState:
        m_sink->resume();
        break;
    case QAudio::StoppedState:
        if (!m_source->isOpen()) {
            qDebug("Opening source");
            bool r=m_source->open(QIODevice::ReadOnly);
            if (!r) {
                qWarning("Open for playback failed");
                return false;
            }
        }

        qDebug("Starting playback");
        m_sink->start(m_source);
        break;
    case QAudio::IdleState:
        qDebug("Idle Audio state");
        //m_audioOutput->resume(m_generator);
        break;
    default:
        qWarning("Unknown Audio state");
        return false;
        break;
    }

    qDebug() << "Buffer size used is: " << m_sink->bufferSize();

    return true;
}

bool CMQtAudioSink::stop()
{
    m_sink->stop();
    m_source->close(); // XXX Should we close the source too ?

    return true;
}

bool CMQtAudioSink::pause()
{
    m_sink->suspend();

    return true;
}

bool CMQtAudioSink::resume()
{
    m_sink->resume();

    return true;
}

void CMQtAudioSink::deviceChanged(int index)
{
    m_source->close();
    m_sink->stop();
    m_sink->disconnect(this);
    createAudioOutput();
}

void CMQtAudioSink::notified()
{
    //qWarning() << "bytesFree = " << m_audioOutput->bytesFree() << ", " << "elapsedUSecs = " << m_audioOutput->elapsedUSecs() << ", " << "processedUSecs = " << m_audioOutput->processedUSecs();
    emit position(m_sink->processedUSecs());
}

void CMQtAudioSink::toggleSuspendResume()
{
    if (m_sink->state() == QAudio::SuspendedState) {
        qDebug() << "status: Suspended, resume()";
        m_sink->resume();
    } else if (m_sink->state() == QAudio::ActiveState) {
        qDebug() << "status: Active, suspend()";
        m_sink->suspend();
    } else if (m_sink->state() == QAudio::StoppedState) {
        qDebug() << "status: Stopped, resume()";
        m_sink->resume();
    } else if (m_sink->state() == QAudio::IdleState) {
        qDebug() << "status: IdleState";
    }
}

void CMQtAudioSink::outputStateChanged(QAudio::State state)
{
    qDebug() << "QtAudioOuputState: " << state;
    setState(state);
}
