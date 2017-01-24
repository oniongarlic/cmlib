#include "cmsc68audiosource.h"

#include <QDebug>

static void
sc68_debug_cb(const int bit, sc68_t * sc68, const char *fmt, va_list list)
{
    qDebug() << fmt;
}

bool CMSC68AudioSource::initOnce=false;

CMSC68AudioSource::CMSC68AudioSource(QObject *parent)
    : CMBaseAudioSource(parent)

{
    sc68_create_t create;

    qDebug("SC68 Constructor");

    if (!initOnce) {
        sc68_init_t init;
        memset(&init,0,sizeof(init));
        init.flags.no_load_config=1;
        init.flags.no_save_config=1;
        //init.msg_handler=sc68_debug_cb;
        if (sc68_init(&init)==-1) {
            qWarning("SC68 init failed");
            qDebug() << sc68_error(0);
            goto error_out;
        }
    }
    initOnce=true;

    memset(&create,0,sizeof(create));
    // create.name = "bb-sc68";
    create.sampling_rate = m_rate;


    m_sc68 = sc68_create(&create);
    if (!m_sc68) {
        qWarning("SC68 create failed");
        qDebug() << sc68_error(0);
        goto error_out;
    }

    qDebug() << "sc68 rate " << create.sampling_rate;

    sc68_cntl(m_sc68, SC68_SET_ASID, SC68_ASID_ON);
    // sc68_cntl(m_sc68, )
    return;

error_out:;
    m_valid=false;
}

CMSC68AudioSource::~CMSC68AudioSource()
{
    if (!m_sc68)
        sc68_destroy(m_sc68);
}

bool CMSC68AudioSource::open(QIODevice::OpenMode mode)
{
    sc68_music_info_t minfo;
    bool r=false;

    switch (mode) {
    case QIODevice::ReadOnly:
        if (m_data.isEmpty()) {
            qWarning("SC68: Not media loaded");
            return false;
        }

        if (sc68_load_mem(m_sc68, m_data.data(), m_data.size())==-1) {
            qWarning("Failed to load SC68 file");
            qDebug() << sc68_error(m_sc68);
            return false;
        }

        if (sc68_play(m_sc68, 1, SC68_DEF_LOOP) == -1) {
            qWarning("Failed to prepare SC68 file for playback");
            qDebug() << sc68_error(m_sc68);
            sc68_close(m_sc68);
            return false;
        }

        if (sc68_process(m_sc68, 0, 0) == SC68_ERROR) {
            qWarning("Failed to prepare SC68 file for processing");
            qDebug() << sc68_error(m_sc68);
            sc68_close(m_sc68);
            return false;
        }

        r=true;

        m_track=sc68_cntl(m_sc68, SC68_GET_DEFTRK);
        m_tracks=sc68_cntl(m_sc68, SC68_GET_TRACKS);

        m_meta.clear();

        if (sc68_music_info(m_sc68, &minfo, m_track, 0)==SC68_OK) {
            m_meta.insert("title", minfo.title);
            m_meta.insert("artist", minfo.artist);
            m_meta.insert("album", minfo.album);
            m_meta.insert("format", minfo.format);
            m_meta.insert("genre", minfo.genre);
        }

        m_meta.insert("tracks", m_tracks);

        emit metaChanged(m_meta);

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

void CMSC68AudioSource::close()
{
    switch (openMode()) {
    case QIODevice::ReadOnly:
        sc68_close(m_sc68);
        break;
    case QIODevice::WriteOnly:
        setvalid(!m_data.isEmpty());
        break;
    default:
        break;
    }

    QIODevice::close();
}

bool CMSC68AudioSource::reset()
{
    return true;
}

QStringList CMSC68AudioSource::extensions()
{
    QStringList e;

    e << "*.sc68" << "*.sndh";

    return e;
}

void CMSC68AudioSource::setTrack(quint16 track)
{
    qDebug() << "sc68 setTrack" << track << m_track << m_tracks;
    if (m_track<m_tracks) {
        sc68_play(m_sc68, track, SC68_DEF_LOOP);
        CMBaseAudioSource::setTrack(track);
    }
}

qint64 CMSC68AudioSource::generateData(qint64 maxlen)
{
    int r,n=maxlen/4;

    r=sc68_process(m_sc68, (void *)m_buffer.data(), &n);
    switch (r) {
    case SC68_OK:
        setPosition(sc68_cntl(m_sc68, SC68_GET_PLAYPOS));
        break;
    case SC68_ERROR:
        qWarning() << "sc68: process error " << r << sc68_error(m_sc68);
        break;
    case SC68_IDLE:
        //
        break;
    default:
        if (r & SC68_CHANGE) {
            qDebug() << "sc68: Track changed";
            setTrack(sc68_cntl(m_sc68, SC68_GET_TRACK));
        }
        if (r & SC68_LOOP) {
            qDebug() << "sc68: Track loop";
        }
        if (r & SC68_END) {
            qDebug() << "sc68: Track end";
        }
        if (r & SC68_SEEK) {
            qDebug() << "sc68: seek";
        }
        return 0;
    }

    return maxlen;
}
