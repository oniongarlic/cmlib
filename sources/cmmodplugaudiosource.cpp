/*
 * Track module source
 *
 *
 */

#include "cmmodplugaudiosource.h"

#include <QDebug>

CMModPlugAudioSource::CMModPlugAudioSource(QObject *parent) :
    CMBaseAudioSource(parent)
{
    ModPlug_GetSettings(&settings);
    settings.mFlags = MODPLUG_ENABLE_OVERSAMPLING;
    settings.mResamplingMode = MODPLUG_RESAMPLE_FIR; /* RESAMP */
    settings.mChannels = m_channels;
    settings.mBits = 16;
    settings.mFrequency = m_rate;
    /* insert more setting changes here */
    ModPlug_SetSettings(&settings);
    m_modplug=NULL;
    m_track_pos=0;
}

qint64 CMModPlugAudioSource::generateData(qint64 maxlen)
{
    qint64 r;

    r=ModPlug_Read(m_modplug, m_buffer.data(), maxlen);

    // Check for end-of-track and if so, zero fill buffer and report
    if (r==0) {
        qDebug("modplug: EOT");
        m_buffer.fill(0);
        emit eot();
        reset();
    } else {        
        float tmp=((float)r/(float)(m_rate*2.0*2.0))*(float)1000.0;
        m_track_pos+=tmp;
        setPosition(m_track_pos);
    }

    m_speed=ModPlug_GetCurrentSpeed(m_modplug);
    m_tempo=ModPlug_GetCurrentTempo(m_modplug);
    m_order=ModPlug_GetCurrentOrder(m_modplug);
    m_pattern=ModPlug_GetCurrentPattern(m_modplug);
    m_row=ModPlug_GetCurrentRow(m_modplug);
    m_pchannels=ModPlug_GetPlayingChannels(m_modplug);

    // qDebug() << m_order << ":" << m_row << "-" << m_pattern;

    return r;
}

qint64 CMModPlugAudioSource::writeData(const char *data, qint64 len)
{
    m_tune.append(data, len);
    qDebug() << "Wrote: " << len;
    return len;
}

bool CMModPlugAudioSource::open(QIODevice::OpenMode mode)
{
    bool r=false;

    switch (mode) {
    case QIODevice::ReadOnly:
        if (m_tune.isEmpty())
            return false;
        m_modplug = ModPlug_Load(m_tune.data(), m_tune.size());
        if (m_modplug) {
            QIODevice::open(mode);
            r=true;            
            setTracks(1);
            setTrack(1);
            m_meta.clear();
            m_meta.insert("title", ModPlug_GetName(m_modplug));
            m_meta.insert("comment", ModPlug_GetMessage(m_modplug));
            m_meta.insert("channels", ModPlug_NumChannels(m_modplug));
            m_meta.insert("instruments", ModPlug_NumInstruments(m_modplug));
            m_meta.insert("patterns", ModPlug_NumPatterns(m_modplug));
            m_meta.insert("samples", ModPlug_NumSamples(m_modplug));

            QVariantList samples;
            for (uint i=0;i<ModPlug_NumSamples(m_modplug);i++) {
                char tmp[80];
                ModPlug_SampleName(m_modplug, i, tmp);
                samples << tmp;
            }

            m_meta.insert("sampleData", samples);

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

QStringList CMModPlugAudioSource::extensions()
{
    QStringList e;

    e << "*.mod" << "*.xm" << "*.it" << "*.s3m" << "*.stm" << "*.669" << "*.amf" << "*.ams" << "*.dbm" << "*.dmf";
    e << "*.dsm" << "*.far" <<"*.mdl" << "*.med" << "*.mtm" <<  "*.okt" << "*.ptm" << "*.ult" << "*.umx" << "*.mt2" << "*.psm";

    return e;
}

void CMModPlugAudioSource::close()
{
    switch (openMode()) {
    case QIODevice::ReadOnly:

        break;
    case QIODevice::WriteOnly:
        setvalid(!m_tune.isEmpty());
        break;
    default:
        break;
    }

    QIODevice::close();
}

bool CMModPlugAudioSource::reset()
{
    if (!m_modplug) {
        qWarning("moduplug: reset without data");
        return false;
    }

    ModPlug_Seek(m_modplug, 0);    
    m_track_pos=0;
    setPosition(m_track_pos);

    return true;
}
