#include "cmfileaudiosink.h"

CMFileAudioSink::CMFileAudioSink(QObject *parent) :
    CMBaseAudioSink(parent),
    m_readSize(44100*2*2)
{
    m_timer.setInterval(100);
    connect(&m_timer, SIGNAL(timeout()), SLOT(readTicker()));
}

bool CMFileAudioSink::play()
{
    bool r;

    if (!m_source) {
        qWarning("Source not set!");
        return false;
    }

    if (m_file.fileName().isEmpty()) {
        qWarning("File name not set");
        return false;
    }

    if (!m_source->isOpen()) {
        qDebug("Opening source");
        r=m_source->open(QIODevice::ReadOnly);
        if (!r) {
            qWarning("Open for playback failed");
            return false;
        }
    }

    if (m_file.isOpen()) {
        m_timer.start();
        setState(QAudio::ActiveState);
        return true;
    }

    r=m_file.open(QIODevice::WriteOnly);

    if (r) {
        afterOpen();
        m_timer.start();
        setState(QAudio::ActiveState);
    } else {
        qWarning("Failed to open file for writing");
    }

    return r;
}

bool CMFileAudioSink::stop()
{
    if (!m_file.isOpen())
        return false;

    aboutToClose();

    m_file.close();
    //m_file.disconnect(this, 0);

    setState(QAudio::StoppedState);

    return true;
}

bool CMFileAudioSink::pause()
{
    m_timer.stop();
    setState(QAudio::SuspendedState);

    return true;
}

bool CMFileAudioSink::setFile(const QString file)
{
    m_file.setFileName(file);

    return true;
}

void CMFileAudioSink::readTicker()
{
    QByteArray tmp=m_source->read(m_readSize);
    m_file.write(tmp);
}

bool CMFileAudioSink::afterOpen()
{
    return true;
}

bool CMFileAudioSink::aboutToClose()
{
    return true;
}
