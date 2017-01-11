#include "cmsc68audiosource.h"

#include <QDebug>

CMSC68AudioSource::CMSC68AudioSource(QObject *parent)
    : CMBaseAudioSource(parent)

{
    sc68_create_t create;

    memset(&create,0,sizeof(create));
    // create.name = "bb-sc68";
    create.sampling_rate = m_rate;

    if (sc68_init(0)==-1)
        qWarning("SC68 init failed");

    m_sc68 = sc68_create(&create);
    if (!m_sc68)
        qWarning("SC68 create failed");

    qDebug() << "sc68 rate " << create.sampling_rate;

    sc68_cntl(0, SC68_SET_ASID, SC68_ASID_ON);
}

QStringList CMSC68AudioSource::extensions()
{
    QStringList e;

    e << "*.sc68";

    return e;
}

bool CMSC68AudioSource::generateData(qint64 maxlen)
{
    int r,n=maxlen/4;
    r=sc68_process(m_sc68, (void *)m_buffer.data(), &n);
    switch (r) {
    case SC68_OK:
        //pos=sc68_cntl(m_sc68, SC68_GET_PLAYPOS)/1000;
        return true;
        break;
    case SC68_ERROR:
        qWarning() << "sc68: process error " << r << sc68_error(m_sc68);
        break;
    default:
        if (r & SC68_CHANGE) {
            qDebug() << "sc68: Track changed";
            m_track=sc68_cntl(m_sc68, SC68_GET_TRACK);
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
    }
    return false;
}
