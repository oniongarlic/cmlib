/*
 * SAP Source
 *
 */

#include "cmsapaudiosource.h"

#include <QDebug>

CMSapAudioSource::CMSapAudioSource(QObject *parent)
    : CMBaseAudioSource(parent)
{
    m_asap = ASAP_New();
}

CMSapAudioSource::~CMSapAudioSource()
{
    ASAP_Delete(m_asap);
}

qint64 CMSapAudioSource::generateData(qint64 maxlen)
{
    m_buffer_length=ASAP_Generate(m_asap, (unsigned char *)m_buffer.data(), m_channels==1 ? maxlen/2 : maxlen, ASAPSampleFormat_S16_L_E);
    setPosition(ASAP_GetPosition(m_asap));

    if (m_buffer_length==0) {
        qDebug("sap: EOT");
        emit eot();
    }

    // We deal with stereo, always but ASAP will generate mono data if only one track so adjust
    if (m_channels==1 && m_buffer_length>0) {
        for (int i=m_buffer_length-1;i>=2;i-=2){
            m_buffer[i*2+1]=m_buffer[i];
            m_buffer[i*2]=m_buffer[i-1];

            m_buffer[i*2-1]=m_buffer[i];
            m_buffer[i*2-2]=m_buffer[i-1];

        }
    }

    return m_buffer_length;
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
        if (m_data.isEmpty()) {
            qWarning("No SAP data loaded");
            return false;
        }

        if (m_data.size()>ASAPInfo_MAX_MODULE_LENGTH) {
            qWarning("SAP Data too big!");
            return false;
        }

        if (!ASAP_Load(m_asap, NULL,(const unsigned char *)m_data.data(), m_data.size())) {
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
        m_meta.insert("channels", m_channels);

        emit metaChanged(m_meta);

        QIODevice::open(mode);
        m_pos=0;
        ASAP_DetectSilence(m_asap, 2);
        r=ASAP_PlaySong(m_asap, m_track-1, -1);

        break;
    case QIODevice::WriteOnly:
        m_data.clear();
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
        setvalid(!m_data.isEmpty());
        break;
    case QIODevice::WriteOnly:
        setvalid(!m_data.isEmpty());
        break;
    default:
        break;
    }

    QIODevice::close();
}

bool CMSapAudioSource::reset()
{
    ASAP_Seek(m_asap, 0);
    m_pos=0;
    return true;
}

QStringList CMSapAudioSource::extensions()
{
    QStringList e;

    e << "*.sap" <<  "*.rmt" << "*.cmc" << "*.cm3" << "*.cmr" << "*.cms" << "*.dmc" << "*.dlt" << "*.mpt" << "*.mpd" << "*.tmc" << "*.tm8" << "*.tm2" << "*.fc";

    return e;
}
