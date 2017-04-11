#include "cmayaudiosource.h"

#include <QDebug>

QStringList CMAYAudioSource::extensions()
{
    QStringList e;

    e << "*.vtx";

    return e;
}

CMAYAudioSource::CMAYAudioSource(QObject *parent)
    : CMBaseAudioSource(parent)
    , m_vtx(0)
    , m_aypos(0)
{
    ayemu_init(&m_ay);
    ayemu_set_sound_format(&m_ay, m_rate, 2, 16);
}

qint64 CMAYAudioSource::generateData(qint64 maxlen)
{
    if (m_aypos>m_vtx->frames) {
        emit eot();
        ayemu_reset(&m_ay);
        return 0;
    }

    // XXX: We read one frame, sized m_fsize at a time only
    // try to generate up to maxlen instead
    // also handle case where fsize is larger than maxlen

    ayemu_vtx_getframe (m_vtx, m_aypos, m_regs);
    ayemu_set_regs (&m_ay, m_regs);
    ayemu_gen_sound (&m_ay, (void *)m_buffer.data(), m_fsize);
    m_aypos++;

    return m_fsize;
}

bool CMAYAudioSource::open(QIODevice::OpenMode mode)
{
    bool r=false;

    switch (mode) {
    case QIODevice::ReadOnly:
        if (m_data.isEmpty()) {
            qWarning("AY: Not media loaded");
            return false;
        }

        m_vtx=ayemu_vtx_load(m_data.data(), m_data.size());
        if (!m_vtx) {
            qWarning("AY: Failed to load VTX file");
            return false;
        }

        r=true;

        m_track=1;
        m_tracks=1;

        m_aypos=0;

        m_meta.clear();

        m_meta.insert("title", m_vtx->title);
        m_meta.insert("artist", m_vtx->author);
        m_meta.insert("comment", m_vtx->comment);
        m_meta.insert("format", m_vtx->from);
        // m_meta.insert("genre", minfo.genre);
        m_meta.insert("year", m_vtx->year);

        m_meta.insert("tracks", m_tracks);

        emit metaChanged(m_meta);

        ayemu_reset(&m_ay);
        ayemu_set_chip_type(&m_ay, m_vtx->chiptype, NULL);
        ayemu_set_chip_freq(&m_ay, m_vtx->chipFreq);
        ayemu_set_stereo(&m_ay, (ayemu_stereo_t)m_vtx->stereo, NULL);

        m_fsize=m_rate * 2 * (16 >> 3) / m_vtx->playerFreq;
        m_frame.resize(m_fsize);

        qDebug() << "FrameSize" << m_fsize;

        if (r) {
            QIODevice::open(mode);
            m_pos=0;
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

void CMAYAudioSource::close()
{
    switch (openMode()) {
    case QIODevice::ReadOnly:
        if (m_vtx) {
            ayemu_vtx_free(m_vtx);
            m_vtx=NULL;
        }
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

bool CMAYAudioSource::reset()
{
    m_aypos=0;
    ayemu_reset(&m_ay);
}
