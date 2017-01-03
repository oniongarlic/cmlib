#include "cmbaseaudiosource.h"

#include <QDebug>

#define DEFAULT_CHANNELS (2)
#define DEFAULT_RATE (44100)

CMBaseAudioSource::CMBaseAudioSource(QObject *parent) :
    QIODevice(parent),
    m_pos(0),
    m_track(0),
    m_tracks(0),
    m_channels(DEFAULT_CHANNELS),
    m_rate(DEFAULT_RATE),
    m_valid(false)
{

}

qint64 CMBaseAudioSource::bytesAvailable() const
{
    return m_buffer.size() + QIODevice::bytesAvailable();
}

bool CMBaseAudioSource::canReadLine() const
{
    return false;
}

void CMBaseAudioSource::setChannels(quint8 channels)
{
    if (m_channels == channels)
        return;

    m_channels = channels;
    emit channelsChanged(channels);
}

void CMBaseAudioSource::setRate(quint32 rate)
{
    if (m_rate == rate)
        return;

    m_rate = rate;
    emit rateChanged(rate);
}

void CMBaseAudioSource::setTrack(quint16 track)
{
    if (m_track == track)
        return;

    m_track = track;
    emit trackChanged(track);
}

void CMBaseAudioSource::setTracks(quint16 tracks)
{
    if (m_tracks == tracks)
        return;

    m_tracks = tracks;
    emit tracksChanged(tracks);
}

void CMBaseAudioSource::setPosition(quint64 position)
{    
    if (m_position == position)
        return;    

    m_position = position;
    emit positionChanged(position);
}

void CMBaseAudioSource::setvalid(bool valid)
{
    if (m_valid == valid)
        return;

    m_valid = valid;
    emit validChanged(valid);
}

qint64 CMBaseAudioSource::readData(char *data, qint64 maxlen)
{
    qint64 total = 0;

    bool r=generateData(maxlen);

    if (r==false) {
        qWarning("Source failed to generate data");
        return -1;
    }

    while (maxlen - total > 0) {
        const qint64 chunk = qMin((m_buffer.size() - m_pos), maxlen - total);
        memcpy(data + total, m_buffer.constData() + m_pos, chunk);
        m_pos = (m_pos + chunk) % m_buffer.size();
        total += chunk;
    }

    // qDebug() << r << maxlen << total;

    return total;
}

qint64 CMBaseAudioSource::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    qWarning("Data write not implemented in this element");

    return -1;
}
