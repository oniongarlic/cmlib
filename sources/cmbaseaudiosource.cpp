#include "cmbaseaudiosource.h"

#include <QDebug>

#define DEFAULT_CHANNELS (2)
#define DEFAULT_RATE (48000)

CMBaseAudioSource::CMBaseAudioSource(QObject *parent) :
    QIODevice(parent),
    m_buffer(DEFAULT_RATE*DEFAULT_CHANNELS*2, (char)0),
    m_buffer_length(0),
    m_pos(0),
    m_track(0),
    m_tracks(0),
    m_channels(DEFAULT_CHANNELS),
    m_rate(DEFAULT_RATE),
    m_valid(false)
{
    qDebug("baseSource");
    qDebug() << m_buffer.size();
}

qint64 CMBaseAudioSource::bytesAvailable() const
{
    return m_buffer_length + QIODevice::bytesAvailable();
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

    if (!isOpen()) {
        qWarning("source: Trying to read closed source");
        return -1;
    }

    if (maxlen==0) {
        qWarning("source: Zero read");
        return 0;
    }

    m_buffer_length=generateData(qMin((qint64)m_buffer.size(), maxlen));

    if (m_buffer_length==0) {
        qWarning("Source failed to generate data");
        return 0;
    }

    while (m_buffer_length - total > 0) {
        const qint64 chunk = qMin((m_buffer_length - m_pos), m_buffer_length - total);
        memcpy(data + total, m_buffer.constData() + m_pos, chunk);
        m_pos = (m_pos + chunk) % m_buffer_length;
        total += chunk;
    }

    return total;
}

qint64 CMBaseAudioSource::writeData(const char *data, qint64 len)
{
    m_data.append(data, len);
    qDebug() << "SOURCE data: " << len;
    return len;
}
