#include <QDebug>

#include "cmmodplugaudiosource.h"

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
}

bool CMModPlugAudioSource::generateData(qint64 maxlen)
{
    int r;

    qint64 length = maxlen;
    m_buffer.resize(length);

    r=ModPlug_Read(m_modplug, m_buffer.data(), length);

    if (r==0) {
        ModPlug_Seek(m_modplug, 0);
    } else {
        //m_pos+=(float)r/(float)(m_rate*2*2);
    }

    return r>0 ? true : false;
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
            m_pos=0;
        }
        break;
    case QIODevice::WriteOnly:
        m_tune.clear();
        r=true;
        QIODevice::open(mode);
        break;
    case QIODevice::ReadWrite:
        qWarning("ReadWrite is not supported");
        return false;
        break;
    }

    return r;
}

void CMModPlugAudioSource::close()
{
    QIODevice::close();
}

bool CMModPlugAudioSource::reset()
{
    if (!m_modplug)
        return false;

    ModPlug_Seek(m_modplug, 0);
    m_pos=0;

    return true;
}
