#include "cmsapaudiosource.h"

#include <QDebug>

CMSapAudioSource::CMSapAudioSource(QObject *parent)
    : CMBaseAudioSource(parent)
{
    m_asap = ASAP_New();
}

bool CMSapAudioSource::generateData(qint64 maxlen)
{
    qint64 length = maxlen;
    m_buffer.resize(length);
    int played=ASAP_Generate(m_asap, (unsigned char *)m_buffer.data(), length, ASAPSampleFormat_S16_L_E);
    // m_pos=ASAP_GetPosition(m_asap)/1000;

    return played>0 ? true : false;
}

qint64 CMSapAudioSource::writeData(const char *data, qint64 len)
{
    m_tune.append(data, len);
    qDebug() << "Wrote: " << len;
    return len;
}

void CMSapAudioSource::setTrack(quint16 track)
{
    ASAP_PlaySong(m_asap, track-1, -1);
    CMBaseAudioSource::setTrack(track);
}

bool CMSapAudioSource::open(QIODevice::OpenMode mode)
{
    bool r=false;

    switch (mode) {
    case QIODevice::ReadOnly:
        if (m_tune.isEmpty())
            return false;

        if (!ASAP_Load(m_asap, NULL,(const unsigned char *)m_tune.data(), m_tune.size())) {
            qWarning("Failed to load SAP file");
            return false;
        }
        m_info = ASAP_GetInfo(m_asap);
        m_tracks=ASAPInfo_GetSongs(m_info);
        m_track = ASAPInfo_GetDefaultSong(m_info)+1;
        m_channels=ASAPInfo_GetChannels(m_info);

        m_meta.clear();
        m_meta.insert("title", ASAPInfo_GetTitleOrFilename(m_info));
        m_meta.insert("author", ASAPInfo_GetAuthor(m_info));
        m_meta.insert("tracks", m_tracks);

        emit metaChanged(m_meta);

        QIODevice::open(mode);
        m_pos=0;
        ASAP_PlaySong(m_asap, m_track-1, -1);

        break;
    case QIODevice::WriteOnly:
        m_tune.clear();
        QIODevice::open(mode);
        break;
    case QIODevice::ReadWrite:
        qWarning("ReadWrite is not supported");
        return false;
        break;
    }

    return r;
}

void CMSapAudioSource::close()
{
    switch (openMode()) {
    case QIODevice::ReadOnly:
        m_info=NULL;
        break;
    case QIODevice::WriteOnly:
        setvalid(!m_tune.isEmpty());
        break;
    default:
        break;
    }

    QIODevice::close();
}
