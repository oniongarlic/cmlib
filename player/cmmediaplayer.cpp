#include <QFile>
#include <QDebug>

#include "cmmediaplayer.h"

#define CHECK_SINK(s) { if (!s) { qWarning("Sink is not set"); return false; } }
#define CHECK_SOURCE(s) { if (!s) { qWarning("Source is not set"); return false; } }

CMMediaPlayer::CMMediaPlayer(QObject *parent)
    : QObject(parent)
{
    connect(&m_dec, SIGNAL(metadata(QVariantHash)), this, SLOT(decoderMetadata(QVariantHash)));
}

/**
 * @brief CMMediaPlayer::load
 * @param file
 * @return
 */
bool CMMediaPlayer::load(const QString file)
{
    CMBaseAudioSource *source=m_dec.findSuitableDecoder(file);
    if (!source)
        return false;

    qDebug() << "Loading file " << file;

    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QByteArray data=f.readAll();

    if (data.isEmpty())
        return false;

    return load(data, source);
}

/**
 * @brief CMMediaPlayer::load
 * @param data
 * @param source
 * @return
 */
bool CMMediaPlayer::load(const QByteArray &data, CMBaseAudioSource *source)
{
    qDebug() << "Loading data " << data.size();

    if (m_sink && m_sink->state()!=QAudio::StoppedState)
        return false;

    m_source=source;

    if (m_source->isOpen())
        m_source->close();

    m_source->open(QIODevice::WriteOnly);
    m_source->write(data.constData(), data.size());
    m_source->close();

    m_sink->setAudioSource(m_source);

    return true;
}

bool CMMediaPlayer::play()
{
    qDebug("Play");

    CHECK_SINK(m_sink);
    CHECK_SOURCE(m_source);

    return m_sink->play();
}

bool CMMediaPlayer::stop()
{
    qDebug("Stop");

    CHECK_SINK(m_sink);
    CHECK_SOURCE(m_source);

    return m_sink->stop();
}

bool CMMediaPlayer::pause()
{
    qDebug("Pause");

    CHECK_SINK(m_sink);
    CHECK_SOURCE(m_source);

    return m_sink->pause();
}

bool CMMediaPlayer::setTrack(quint16 track)
{
    m_source->setTrack(track);
    return true;
}

bool CMMediaPlayer::setAudioSink(CMBaseAudioSink *sink)
{
    m_sink=sink;

    return true;
}

QAudio::State CMMediaPlayer::getState()
{
    return m_sink->state();
}

void CMMediaPlayer::decoderMetadata(QVariantHash meta)
{
    emit metadata(meta);
}

void CMMediaPlayer::sinkPosition(double pos)
{
    m_position=pos;
    emit positionChanged(m_position);
}
