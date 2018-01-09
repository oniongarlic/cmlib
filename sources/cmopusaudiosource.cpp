#include "cmopusaudiosource.h"

CMOpusAudioSource::CMOpusAudioSource(QObject *parent)
    : CMBaseAudioSource(parent),
      m_opus(NULL),
      m_track_pos(0)
{

}

CMOpusAudioSource::~CMOpusAudioSource()
{    
    if (m_opus)
        op_free(m_opus);
    m_data.clear();
}

QStringList CMOpusAudioSource::extensions()
{
    QStringList e;

    e << "*.opus";

    return e;
}

qint64 CMOpusAudioSource::generateData(qint64 maxlen)
{
    int r;

    r=op_read_stereo(m_opus, (opus_int16 *)m_buffer.data(), maxlen);

    if (r>=0) {
        float tmp=((float)r*4/(float)(48000*2.0*2.0))*(float)1000.0;
        m_track_pos+=tmp;
        setPosition(m_track_pos);
        return r*4;
    }

    if (r==OP_HOLE)
        return 0;

    qWarning() << "Opus: Read error " << r;

    return -1;
}

bool CMOpusAudioSource::open(QIODevice::OpenMode mode)
{
    bool r=false;
    int op;
    const OpusTags *tags;

    switch (mode) {
    case QIODevice::ReadOnly:
        if (m_data.isEmpty() || m_data.size()<512) {
            qWarning("Opus: No media loaded");
            return false;
        }

        m_buffer.fill(0);
        m_buffer_length=0;
        m_track_pos=0.0;
        setPosition(m_track_pos);

        m_opus=op_open_memory((const unsigned char *)m_data.constData(), m_data.size(), &op);

        if (op!=0) {
            qWarning("Opus: Not valid opus data");
            qDebug() << op;
            return false;
        } else {
            QIODevice::open(mode);
            r=true;
        }

        setTracks(1);

        m_link=op_current_link(m_opus);

        qDebug() << m_link;

        m_meta.clear();
        tags=op_tags(m_opus, m_link);
        if (tags && tags->comments>0) {            

            m_meta.insert("vendor", tags->vendor);

            for(uint i=0; i<tags->comments; i++){
                QString tmp(tags->user_comments[i]);
                int sep=tmp.indexOf('=');

                m_meta.insert(tmp.mid(0, sep).toLower(), tmp.mid(sep+1));
            }
        }

        m_meta.insert("links", op_link_count(m_opus));
        m_meta.insert("channels", op_channel_count(m_opus, m_link));
        m_meta.insert("serialno", op_serialno(m_opus, m_link));

        m_bitrate=op_bitrate(m_opus, m_link);
        m_meta.insert("bitrate", m_bitrate);

        emit metaChanged(m_meta);

        break;
    case QIODevice::WriteOnly:
        m_data.clear();
        QIODevice::open(mode);
        r=true;
        break;
    case QIODevice::ReadWrite:
        qWarning("ReadWrite is not supported");
        return false;
        break;
    }

    return r;
}

void CMOpusAudioSource::close()
{
    switch (openMode()) {
    case QIODevice::ReadOnly:
        //m_read_buffer.close();
        op_free(m_opus);
        m_opus=NULL;
        m_data.clear();
        break;
    case QIODevice::WriteOnly:
        setvalid(!m_data.isEmpty());
        break;
    default:
        break;
    }

    QIODevice::close();
}

bool CMOpusAudioSource::reset()
{
    if (op_seekable(m_opus)) {
        op_raw_seek(m_opus, 0);
        setPosition(0);
        return true;
    }
    return false;
}
