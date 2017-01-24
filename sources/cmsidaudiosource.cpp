/*
 * SID source
 *
 */

#include "cmsidaudiosource.h"

#include <QDebug>

static QString clockSpeed(clock_t c)
{
    switch (c) {
    case SidTuneInfo::CLOCK_UNKNOWN:
        return "Unknown";
    case SidTuneInfo::CLOCK_PAL:
        return "PAL";
    case SidTuneInfo::CLOCK_NTSC:
        return "NYSC";
    default:
        return "Any";
    }
}

CMSidAudioSource::CMSidAudioSource(QObject *parent) :
    CMBaseAudioSource(parent)
{
    engine = new sidplayfp();
    config = engine->config();

    //const int channelBytes = format.sampleSize() / 8;
    //const int sampleBytes = format.channels() * channelBytes;

    config.defaultSidModel = SidConfig::MOS6581;
    config.defaultC64Model = SidConfig::PAL;
    config.samplingMethod = SidConfig::RESAMPLE_INTERPOLATE;
    config.frequency = m_rate;
    config.playback = m_channels==2 ? SidConfig::STEREO :SidConfig::MONO;

    tune = new SidTune(NULL);
    rs = new ReSIDfpBuilder("ReSIDfp");
    config.sidEmulation = rs;
    rs->create(2);

    engine->config(config);
}

CMSidAudioSource::~CMSidAudioSource()
{
    engine->stop();
    delete tune;
    delete engine;
    delete rs;
}

qint64 CMSidAudioSource::generateData(qint64 maxlen)
{
    m_buffer_length = engine->play((short*)m_buffer.data(), maxlen/2); // 16
    setPosition(engine->time()*1000);

    return m_buffer_length*2;
}

void CMSidAudioSource::setTrack(quint16 track)
{    
    int r=tune->selectSong(track);
    if (r!=0) {
        CMBaseAudioSource::setTrack(r);
        reset();
    }
    qDebug() << r << tune->statusString();
}

bool CMSidAudioSource::prepareTune()
{
    tune->read((const uint_least8_t*)m_data.constData(), m_data.size());
    if (!tune->getStatus()) {
        qWarning("SID: Failed to read data");
        return false;
    }

    const SidTuneInfo *info=tune->getInfo();

    setTracks(info->songs());
    setTrack(0);

    info=tune->getInfo(0);

    m_meta.clear();
    m_meta.insert("title", info->infoString(0));
    m_meta.insert("author", info->infoString(1));
    m_meta.insert("tracks", m_tracks);
    m_meta.insert("stereo", info->isStereo());
    m_meta.insert("format", info->formatString());
    m_meta.insert("clock", clockSpeed(info->clockSpeed()));

    emit metaChanged(m_meta);

    return true;
}



bool CMSidAudioSource::open(QIODevice::OpenMode mode)
{
    bool r=false;

    switch (mode) {
    case QIODevice::ReadOnly:
        if (m_data.isEmpty()) {
            qWarning("SID: Not media loaded");
            return false;
        }
        if (!prepareTune())
            return false;
        r=engine->load(tune);
        if (r) {
            QIODevice::open(mode);
            engine->fastForward(100);
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

void CMSidAudioSource::close()
{
    switch (openMode()) {
    case QIODevice::ReadOnly:
        engine->stop();
        break;
    case QIODevice::WriteOnly:
        setvalid(!m_data.isEmpty());
        break;
    default:
        break;
    }

    QIODevice::close();
}

bool CMSidAudioSource::reset()
{
    engine->stop();
    engine->fastForward(100);
    setPosition(0);
    return true;
}

QStringList CMSidAudioSource::extensions()
{
    QStringList e;

    e << "*.sid";

    return e;
}
