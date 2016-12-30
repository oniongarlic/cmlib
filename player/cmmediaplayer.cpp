#include <QFile>
#include <QDebug>

#include "cmmediaplayer.h"

#define CHECK_SINK(s) { if (!s) { qWarning("Sink is not set"); return false; } }

CMMediaPlayer::CMMediaPlayer(QObject *parent) : QObject(parent)
{

}

bool CMMediaPlayer::load(const QString file)
{
    m_source=m_dec.findSuitableDecoder(file);
    if (!m_source)
        return false;

    qDebug() << "Loading file " << file;

    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QByteArray data=f.readAll();

    if (data.isEmpty())
        return false;

    return load(data);
}

bool CMMediaPlayer::load(const QByteArray &data)
{
    qDebug() << "Loading data " << data.size();

    m_source->open(QIODevice::WriteOnly);
    m_source->write(data.constData(), data.size());
    m_source->close();

    m_sink->setAudioSource(m_source);

    return true;
}

bool CMMediaPlayer::play()
{
    CHECK_SINK(m_sink);

    qDebug("Play");

    return m_sink->play();
}

bool CMMediaPlayer::stop()
{
    CHECK_SINK(m_sink);

    qDebug("Stop");

    return m_sink->stop();
}

bool CMMediaPlayer::pause()
{
    CHECK_SINK(m_sink);

    qDebug("Pause");

    return m_sink->pause();
}

bool CMMediaPlayer::setAudioSink(CMBaseAudioSink *sink)
{
    m_sink=sink;

    return true;
}
