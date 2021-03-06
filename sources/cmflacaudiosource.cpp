/*
 * FLAC Decoder source
 *
 * A Very basic FLAC decoder source. Supports only 16-bit and 2-channel flac files.
 *
 */

#include "cmflacaudiosource.h"
#include <QDebug>

CMFlacAudioSource::CMFlacAudioSource(QObject *parent)
    : CMBaseAudioSource(parent)
{
    init();
    set_metadata_respond(FLAC__METADATA_TYPE_VORBIS_COMMENT);
}

CMFlacAudioSource::~CMFlacAudioSource()
{
    finish();
}

FLAC__StreamDecoderReadStatus CMFlacAudioSource::read_callback(FLAC__byte buffer[], size_t *bytes)
{    
    qint64 r;

    if (m_read_buffer.atEnd())
        return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

    r=m_read_buffer.read((char *)buffer, *bytes);

    if (m_read_buffer.atEnd())
        m_eot=true;

    if (r<=0)
        return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderWriteStatus CMFlacAudioSource::write_callback(const FLAC__Frame *frame, const FLAC__int32 * const buffer[])
{
    const FLAC__uint32 total_size = (FLAC__uint32)(m_total_samples * m_channels * (m_bps/8));
    qint32 fsize=frame->header.blocksize*m_channels*2;

    if (fsize>m_buffer.size()) {
        qDebug() << "Buffer too small, resizing buffer " << fsize;
        m_buffer.resize(fsize);
    }

    qint16 *b=(qint16 *)m_buffer.data();

    for (uint i = 0; i < frame->header.blocksize; i++) {
        for (uint j = 0; j < m_channels; j++) {
            *b++ = (qint16) buffer[j][i];
        }
    }

    m_buffer_length=fsize;

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void CMFlacAudioSource::error_callback(FLAC__StreamDecoderErrorStatus status)
{
    qDebug() << "FLAC: Error CB " << FLAC__StreamDecoderErrorStatusString[status];
}

qint64 CMFlacAudioSource::readData(char *data, qint64 maxlen)
{
    bool r;

    if (m_buffer_length==0) {
        generateData(maxlen);
        m_pos=0;
    } else if (m_pos<m_buffer_length) {

    } else {
        generateData(maxlen);
        m_pos=0;
    }

    qint64 s=qMin(m_buffer_length-m_pos, maxlen);
    memcpy(data, m_buffer.constData()+m_pos, s);
    m_pos+=s;

    return s;
}

qint64 CMFlacAudioSource::generateData(qint64 maxlen)
{  
#if 0
    qint64 len=qMin((qint64)m_buffer.size(), maxlen);

    while (m_buffer_length<len/2) {
        bool r=process_single();
        if (!r)
            qWarning("Failed to process FLAC data");
    }
#else
    bool r=process_single();    
    // XXX: For some reason process_single return true even if we are eof so work around it
    if (!r || m_eot) {
        qDebug() << "FLAC: EOT";      
        emit eot();
    }
#endif

    if (m_buffer_length>0) {
        float tmp=((float)m_buffer_length/(float)(m_rate*2.0*2.0))*(float)1000.0;
        m_track_pos+=tmp;
        setPosition(m_track_pos);
    } else {

    }

    return m_buffer_length;
}

bool CMFlacAudioSource::open(QIODevice::OpenMode mode)
{
    bool r=false;

    switch (mode) {
    case QIODevice::ReadOnly:
        if (m_data.isEmpty()) {
            qWarning("FLAC: Not media loaded");
            return false;
        }

        m_buffer.fill(0);
        m_buffer_length=0;
        m_track_pos=0;
        m_eot=false;

        m_read_buffer.setData(m_data);
        m_read_buffer.open(QIODevice::ReadOnly);

        r=process_until_end_of_metadata();

        if (r && m_channels==2 && m_bps==16 && m_sample_rate==44100) {
            setvalid(true);
            QIODevice::open(mode);
            m_pos=0;

            setTracks(1);

            m_length=((float)get_total_samples()/(float)(m_rate*2.0*2.0))*(float)1000.0;

            qDebug() << m_length;

            m_meta.clear();
            m_meta.insert("channels", m_channels);
            m_meta.insert("rate", m_sample_rate);
            m_meta.insert("bits", m_bps);
            emit metaChanged(m_meta);

        } else if (r) {
            qWarning("FLAC: Only 44.1 kHz, 16-bit, Stereo FLAC files are supported at this time.");
            setvalid(false);
        } else {
            qWarning("FLAC: Failed to process initial flac data");
            setvalid(false);
        }
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

void CMFlacAudioSource::close()
{
    switch (openMode()) {
    case QIODevice::ReadOnly:
        m_read_buffer.close();
        break;
    case QIODevice::WriteOnly:
        setvalid(!m_data.isEmpty());
        break;
    default:
        break;
    }

    QIODevice::close();
}

bool CMFlacAudioSource::reset()
{
    m_eot=false;
    m_track_pos=0;
    setPosition(m_track_pos);
    FLAC::Decoder::Stream::reset();
    return m_read_buffer.seek(0);
}

void CMFlacAudioSource::metadata_callback(const FLAC__StreamMetadata *metadata)
{    
    switch (metadata->type) {
    case FLAC__METADATA_TYPE_STREAMINFO:
        m_total_samples = metadata->data.stream_info.total_samples;
        m_sample_rate = metadata->data.stream_info.sample_rate;
        m_channels = metadata->data.stream_info.channels;
        m_bps = metadata->data.stream_info.bits_per_sample;
        qDebug() << m_total_samples << m_sample_rate << m_channels << m_bps;
        break;
    case FLAC__METADATA_TYPE_VORBIS_COMMENT:
        qWarning() << "FLAC: Comments/Tags not yet handled";
        qDebug() << metadata->data.vorbis_comment.num_comments;
        break;
    default:
        qWarning() << "FLAC: Unhandled metadata type" << metadata->type;
    }
}

bool CMFlacAudioSource::eof_callback()
{
    return m_read_buffer.atEnd();
}

