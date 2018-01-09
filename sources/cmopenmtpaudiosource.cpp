/*
 * Track module source
 *
 *
 */

#include "cmopenmtpaudiosource.h"

#include <QDebug>

CMOpenMTPAudioSource::CMOpenMTPAudioSource(QObject *parent) :
    CMBaseAudioSource(parent)
{
    m_track_pos=0;
}

qint64 CMOpenMTPAudioSource::generateData(qint64 maxlen)
{
    qint64 r;

    r=mod->read_interleaved_stereo((std::int32_t)m_rate, (std::size_t)maxlen/4, (std::int16_t *)m_buffer.data() );

    r=r*4;

    // Check end of track, but if we have more subsongs, switch song instead
    if (r==0 && m_tracks>1 && m_track<m_tracks) {
        // reset();
        setTrack(m_track+1);
        // emit eoss();
    } else if (r==0) { // Check for end-of-track and if so, zero fill buffer and report
        qDebug("openmtp: EOT");
        m_buffer.fill(0);
        emit eot();
    }
    setPosition(mod->get_position_seconds()*1000.0);

    return r;
}

qint64 CMOpenMTPAudioSource::writeData(const char *data, qint64 len)
{
    m_tune.append(data, len);
    qDebug() << "Wrote: " << len;
    return len;
}

void CMOpenMTPAudioSource::setTrack(quint16 track)
{
    if (track>m_tracks) {
        qWarning() << "openmtp: Request track outside tracks!";
        return;
    }
    mod->select_subsong(track-1);
    CMBaseAudioSource::setTrack(track);
}

bool CMOpenMTPAudioSource::open(QIODevice::OpenMode mode)
{
    bool r=false;

    switch (mode) {
    case QIODevice::ReadOnly:
        if (m_tune.isEmpty())
            return false;
        try {
            mod=new openmpt::module((const char *)m_tune.data(), (std::size_t)m_tune.size());
        } catch ( const std::exception & e ) {
            qWarning() << "openmtp exception" << e.what();
            mod=NULL;
        }
        if (mod) {
            QIODevice::open(mode);
            r=true;
            setTracks(mod->get_num_subsongs());
            setTrack(1);
            m_meta.clear();

            m_meta.insert("tracks", m_tracks);
            m_meta.insert("channels", mod->get_num_channels());
            m_meta.insert("instruments", mod->get_num_instruments());
            m_meta.insert("patterns", mod->get_num_patterns());
            m_meta.insert("samples", mod->get_num_samples());
            m_meta.insert("title", QString::fromStdString(mod->get_metadata("title")));
            m_meta.insert("artist", QString::fromStdString(mod->get_metadata("artist")));
            m_meta.insert("tracker", QString::fromStdString(mod->get_metadata("tracker")));
            m_meta.insert("comment", QString::fromStdString(mod->get_metadata("message")));
#if 0
            QVariantList samples;
            for (uint i=0;i<ModPlug_NumSamples(m_modplug);i++) {
                char tmp[80];
                ModPlug_SampleName(m_modplug, i, tmp);
                samples << tmp;
            }

            m_meta.insert("sampleData", samples);
#endif
            emit metaChanged(m_meta);
        } else {
            qWarning("Failed to load mod file, invalid ?");
            r=false;
        }
        break;
    case QIODevice::WriteOnly:
        m_tune.clear();
        r=true;
        QIODevice::open(mode);
        m_track_pos=0;
        setPosition(m_track_pos);
        break;
    case QIODevice::ReadWrite:
        qWarning("ReadWrite is not supported");
        r=false;
        break;
    }

    return r;
}

void CMOpenMTPAudioSource::close()
{
    switch (openMode()) {
    case QIODevice::ReadOnly:
        delete mod;
        mod=NULL;
        break;
    case QIODevice::WriteOnly:
        setvalid(!m_tune.isEmpty());
        break;
    default:
        break;
    }

    QIODevice::close();
}

bool CMOpenMTPAudioSource::reset()
{      
    mod->set_position_seconds(0.0);
    m_track_pos=0;
    setPosition(m_track_pos);

    return true;
}
