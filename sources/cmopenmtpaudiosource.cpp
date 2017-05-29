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

    qDebug() << maxlen << r;

    r=r*4;

    // Check for end-of-track and if so, zero fill buffer and report
    if (r==0) {
        qDebug("openmtp: EOT");
        m_buffer.fill(0);
        emit eot();
        reset();
    } else {        
        float tmp=((float)r/(float)(m_rate*2.0*2.0))*(float)1000.0;
        m_track_pos+=tmp;
        setPosition(m_track_pos);
    }

    return r;
}

qint64 CMOpenMTPAudioSource::writeData(const char *data, qint64 len)
{
    m_tune.append(data, len);
    qDebug() << "Wrote: " << len;
    return len;
}

bool CMOpenMTPAudioSource::open(QIODevice::OpenMode mode)
{
    bool r=false;

    switch (mode) {
    case QIODevice::ReadOnly:
        if (m_tune.isEmpty())
            return false;
        mod=new openmpt::module((const char *)m_tune.data(), (std::size_t)m_tune.size());
        if (mod) {
            QIODevice::open(mode);
            r=true;            
            setTracks(mod->get_num_subsongs());
            setTrack(1);
            m_meta.clear();

            m_meta.insert("channels", mod->get_num_channels());
            m_meta.insert("instruments", mod->get_num_instruments());
            m_meta.insert("patterns", mod->get_num_patterns());
            m_meta.insert("samples", mod->get_num_samples());

            //qDebug() << mod->get_metadata_keys();

#if 0
            //m_meta.insert("title", mod->get_);
            m_meta.insert("comment", ModPlug_GetMessage(m_modplug));

            QVariantList samples;
            for (uint i=0;i<ModPlug_NumSamples(m_modplug);i++) {
                char tmp[80];
                ModPlug_SampleName(m_modplug, i, tmp);
                samples << tmp;
            }

            m_meta.insert("sampleData", samples);
#endif
            m_meta.insert("tracks", m_tracks);

            qDebug() << m_meta;

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

QStringList CMOpenMTPAudioSource::extensions()
{
    QStringList e;

    e << "*.mod" << "*.xm" << "*.it" << "*.s3m" << "*.stm" << "*.669" << "*.amf" << "*.ams" << "*.dbm" << "*.dmf";
    e << "*.dsm" << "*.far" <<"*.mdl" << "*.med" << "*.mtm" <<  "*.okt" << "*.ptm" << "*.ult" << "*.umx" << "*.mt2" << "*.psm";

    return e;
}

void CMOpenMTPAudioSource::close()
{
    switch (openMode()) {
    case QIODevice::ReadOnly:
        delete mod;
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
   // ModPlug_Seek(m_modplug, 0);
    m_track_pos=0;
    setPosition(m_track_pos);

    return true;
}
