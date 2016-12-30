#include "cmbaseaudiosink.h"

CMBaseAudioSink::CMBaseAudioSink(QObject *parent)
    : QObject(parent)
    , m_state(QAudio::StoppedState)
{

}

void CMBaseAudioSink::setAudioSource(CMBaseAudioSource *source)
{
    m_source=source;
}

QAudio::State CMBaseAudioSink::state() const
{
    return m_state;
}
