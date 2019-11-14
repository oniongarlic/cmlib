#include <QFile>
#include <QDebug>

#include "cmmediaplayer.h"

#define CHECK_SINK(s) { if (!s) { qWarning("Sink is not set"); return false; } }
#define CHECK_SOURCE(s) { if (!s) { qWarning("Source is not set"); return false; } }

CMMediaPlayer::CMMediaPlayer(QObject *parent)
    : QObject(parent)
    , m_sink(0)
    , m_playtime(0)
    , m_playing(false)
    , m_position(0)
    , m_length(0)
    , m_track(0)
    , m_tracks(0)
    , m_scanner(this)
{
    connect(&m_dec, SIGNAL(metadata(QVariantMap)), this, SLOT(decoderMetadata(QVariantMap)));
    connect(&m_dec, SIGNAL(eot()), this, SLOT(decoderEOT()));

    m_scanner.initialize("player_mediadatabase.db");
    m_scanner.setFilters(m_dec.getSupportedExtensions());

    m_scanner.addDefaultPath();
}

/**
 * @brief CMMediaPlayer::load
 * @param file
 * @return
 */
bool CMMediaPlayer::load(const QString file)
{
    CMBaseAudioSource *source=m_dec.findSuitableDecoder(file);
    if (!source)
        return false;

    qDebug() << "Loading file " << file;

    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QByteArray data=f.readAll();

    if (data.isEmpty())
        return false;

    return load(data, source);
}

/**
 * @brief CMMediaPlayer::load
 * @param data
 * @param source
 * @return
 */
bool CMMediaPlayer::load(const QByteArray &data, CMBaseAudioSource *source)
{
    qDebug() << "Loading data " << data.size();

    if (m_sink && m_sink->state()!=QAudio::StoppedState)
        return false;

    m_source=source;

    if (m_source->isOpen())
        m_source->close();

    m_source->disconnect(this, 0);
    connect(m_source, SIGNAL(positionChanged(quint64)), SLOT(sinkPosition(quint64)));

    m_source->open(QIODevice::WriteOnly);
    m_source->write(data.constData(), data.size());
    m_source->close();

    // connect(m_source, SIGNAL(eot()), SLOT(decoderEOT()));

    m_sink->setAudioSource(m_source);

    return true;
}

bool CMMediaPlayer::play()
{
    qDebug("Play");

    CHECK_SINK(m_sink);
    CHECK_SOURCE(m_source);

    if (!m_source->valid()) {
        qWarning("Source data is not valid");
        return false;
    }

    return m_sink->play();
}

bool CMMediaPlayer::stop()
{
    qDebug("Stop");

    CHECK_SINK(m_sink);
    CHECK_SOURCE(m_source);

    m_source->reset();
    return m_sink->stop();
}

bool CMMediaPlayer::pause()
{
    qDebug("Pause");

    CHECK_SINK(m_sink);
    CHECK_SOURCE(m_source);

    return m_sink->pause();
}

bool CMMediaPlayer::prepare()
{
    CHECK_SOURCE(m_source);

    bool r=m_source->open(QIODevice::ReadOnly);
    if (r) {
        m_tracks=m_source->tracks();
        emit tracksChanged(m_tracks);

        m_track=m_source->track();
        emit trackChanged(m_track);
    }
    return r;
}

bool CMMediaPlayer::prevTrack()
{
    if (m_track>1)
        return setTrack(m_track-1);

    return false;
}

bool CMMediaPlayer::nextTrack()
{
    if (m_track<m_tracks)
        return setTrack(m_track+1);
    return false;
}

bool CMMediaPlayer::prevSong()
{

}

bool CMMediaPlayer::nextSong()
{

}

bool CMMediaPlayer::setSong(quint16 song)
{

}

bool CMMediaPlayer::setTrack(quint16 track)
{
    m_source->setTrack(track);
    m_track=track;
    emit trackChanged(m_track);
    return true;
}

void CMMediaPlayer::setPlaytime(quint64 ms)
{
    m_playtime=ms;
}

bool CMMediaPlayer::setAudioSink(CMBaseAudioSink *sink)
{
    if (m_sink) {
        if (m_sink->state()!=QAudio::StoppedState)
            return false;
        m_sink->disconnect(this, 0);
    }

    m_sink=sink;

    connect(m_sink, SIGNAL(stateChanged(QAudio::State)), SLOT(sinkState(QAudio::State)));

    if (m_source)
        m_sink->setAudioSource(m_source);

    return true;
}


QSortFilterProxyModel *CMMediaPlayer::getSongModel()
{
    // return m_scanner.model();
    m_model.setSourceModel(m_scanner.model());
    m_model.setFilterCaseSensitivity(Qt::CaseInsensitive);

    return &m_model;
}

bool CMMediaPlayer::setFilter(const QString filter)
{
    m_model.setFilterFixedString(filter);    
    m_model.invalidate();
    //m_model->setFilterRegExp(QRegExp(filter, Qt::CaseInsensitive, QRegExp::FixedString));

    return true;
}

QVariantMap CMMediaPlayer::get(int index)
{    
    QModelIndex tmp=m_model.mapToSource(m_model.index(index, 0));    

    return m_scanner.model()->get(tmp.row());
}

CMMediaScanner *CMMediaPlayer::getMediaScanner()
{
    return &m_scanner;
}

bool CMMediaPlayer::refreshDatabase(bool force)
{
    bool r=true;

    if (m_scanner.count()==0 || force) {
        r=m_scanner.scanAsync();
    } else {
        m_scanner.model()->refresh();
    }

    return r;
}

QAudio::State CMMediaPlayer::getState()
{
    return m_sink->state();
}

void CMMediaPlayer::decoderMetadata(QVariantMap meta)
{
    qDebug() << "Source sent metadata" << meta;

    if (meta.contains("title")) {
        //m_scanner.updateFile();
    }

    emit metadata(meta);
}

void CMMediaPlayer::decoderEOT()
{
    qDebug() << "Source signaled EoT";
    emit eot();
}

void CMMediaPlayer::sinkPosition(quint64 pos)
{
    m_position=pos;
    emit positionChanged(m_position);

    if (m_playtime>0 && pos>m_playtime)
        stop();
}

void CMMediaPlayer::sinkState(QAudio::State state)
{
    qDebug() << "PlayerSinkState " << state;
    switch (state) {
    case QAudio::ActiveState:
        m_playing=true;
        emit playingChanged(m_playing);
        break;
    case QAudio::SuspendedState:
    case QAudio::IdleState:
    case QAudio::StoppedState:
        m_playing=false;
        emit playingChanged(m_playing);
        break;
    default:
        break;
    }
}
