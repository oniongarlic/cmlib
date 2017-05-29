#include "cmbaseaudiosink.h"

#include <QDebug>

CMBaseAudioSink::CMBaseAudioSink(QObject *parent)
    : QObject(parent)
    , m_state(QAudio::StoppedState)
    , m_valid(true)
    , m_channels(2)
    , m_rate(44100)
{

}

bool CMBaseAudioSink::isValid()
{
    return m_valid;
}

void CMBaseAudioSink::setAudioSource(CMBaseAudioSource *source)
{
    m_source=source;
}

QAudio::State CMBaseAudioSink::state() const
{
    return m_state;
}

void CMBaseAudioSink::setState(QAudio::State state)
{
    qDebug() << "CMBaseAudioSink::state = " << state;

    if (m_state==state)
        return;

    m_state=state;
    emit stateChanged(m_state);
}
