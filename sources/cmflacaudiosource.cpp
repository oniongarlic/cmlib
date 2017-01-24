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
}

CMFlacAudioSource::~CMFlacAudioSource()
{
    finish();
}

FLAC__StreamDecoderReadStatus CMFlacAudioSource::read_callback(FLAC__byte buffer[], size_t *bytes)
{
    qDebug() << "FLAC: Read CB " << *bytes;

    if (m_read_buffer.atEnd())
        return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

    m_read_buffer.read((char *)buffer, *bytes);

    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderWriteStatus CMFlacAudioSource::write_callback(const FLAC__Frame *frame, const FLAC__int32 * const buffer[])
{
    const FLAC__uint32 total_size = (FLAC__uint32)(m_total_samples * m_channels * (m_bps/8));
    qint32 fsize=frame->header.blocksize*m_channels*2;

    qDebug() << "FLAC: Write CB " << frame->header.blocksize;

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
#endif

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

        m_read_buffer.setData(m_data);
        m_read_buffer.open(QIODevice::ReadOnly);

        r=process_until_end_of_metadata();

        if (r) {
            setvalid(true);
            QIODevice::open(mode);
            m_pos=0;
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
    FLAC::Decoder::Stream::reset();
    return m_read_buffer.seek(0);
}

void CMFlacAudioSource::metadata_callback(const FLAC__StreamMetadata *metadata)
{
    qDebug() << "FLAC: Meta CB " << metadata->type;
    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        m_total_samples = metadata->data.stream_info.total_samples;
        m_sample_rate = metadata->data.stream_info.sample_rate;
        m_channels = metadata->data.stream_info.channels;
        m_bps = metadata->data.stream_info.bits_per_sample;

        qDebug() << m_total_samples << m_sample_rate << m_channels << m_bps;
    }
}

QStringList CMFlacAudioSource::extensions()
{
    QStringList e;

    e << "*.flac";

    return e;
}
