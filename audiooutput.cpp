#include <QDebug>

#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QtCore/qmath.h>
#include <QtCore/qendian.h>

#include "audiooutput.h"

const int DataFrequencyHz = 44100;
const int BufferSize      = 65535;

Generator::Generator(QObject *parent)
    :   QIODevice(parent)
    ,   m_pos(0)
{    
    engine = new sidplayfp();
    config = engine->config();

    //const int channelBytes = format.sampleSize() / 8;
    //const int sampleBytes = format.channels() * channelBytes;

    config.defaultSidModel = SidConfig::MOS6581;
    config.defaultC64Model = SidConfig::PAL;
    config.frequency = 44100;
    config.samplingMethod = SidConfig::INTERPOLATE;
    config.playback = SidConfig::STEREO;

    tune=new SidTune(NULL);
    tune->load("test.sid");
    if (!tune->getStatus())
        qWarning("Failed to load SID file");

    const SidTuneInfo *info=tune->getInfo();

    qDebug() << "Songs: " << info->songs();

    rs = new ReSIDfpBuilder("ReSIDfp");
    config.sidEmulation = rs;
    rs->create(2);

    engine->config(config);

    qDebug() << tune->statusString();
    tune->selectSong(1);

    qDebug() << tune->statusString();

    engine->load(tune);
    engine->fastForward (100);
}

Generator::~Generator()
{
    engine->stop();
    engine->load(NULL);
}

void Generator::start()
{
    open(QIODevice::ReadOnly);
}

void Generator::stop()
{
    m_pos = 0;
    engine->stop();
    close();
}

void Generator::generateData(qint64 len)
{
    qint64 length = len;
    m_buffer.resize(length);

    qint64 played = engine->play((short*)m_buffer.data(), length/2); // 16
}

qint64 Generator::readData(char *data, qint64 len)
{
    qint64 total = 0;

    generateData(len);

    while (len - total > 0) {
        const qint64 chunk = qMin((m_buffer.size() - m_pos), len - total);
        memcpy(data + total, m_buffer.constData() + m_pos, chunk);
        m_pos = (m_pos + chunk) % m_buffer.size();
        total += chunk;
    }

    qDebug() << engine->time();

    return total;
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 Generator::bytesAvailable() const
{
    return m_buffer.size() + QIODevice::bytesAvailable();
}

AudioTest::AudioTest()
    :   m_pullTimer(new QTimer(this))
    ,   m_device(QAudioDeviceInfo::defaultOutputDevice())
    ,   m_generator(0)
    ,   m_audioOutput(0)
    ,   m_output(0)
    ,   m_buffer(BufferSize, 0)
{
    qDebug("InitAudio");
    initializeAudio();
}

void AudioTest::initializeAudio()
{
    connect(m_pullTimer, SIGNAL(timeout()), SLOT(pullTimerExpired()));

    m_pullMode = true;

    m_format.setFrequency(DataFrequencyHz);
    m_format.setChannels(2);
    m_format.setSampleSize(16);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    qDebug() << info.deviceName();
    if (!info.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = info.nearestFormat(m_format);
    }

    m_generator = new Generator(this);

    createAudioOutput();
}

void AudioTest::createAudioOutput()
{
    m_audioOutput = new QAudioOutput(m_device, m_format, this);
    connect(m_audioOutput, SIGNAL(notify()), SLOT(notified()));
    connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));
    m_generator->start();
    m_audioOutput->setBufferSize(65535*4);
    // m_audioOutput->start(m_generator);
    m_output = m_audioOutput->start();
    m_pullMode = false;
    m_pullTimer->start(100);
}

AudioTest::~AudioTest()
{

}

void AudioTest::deviceChanged(int index)
{
    m_pullTimer->stop();
    m_generator->stop();
    m_audioOutput->stop();
    m_audioOutput->disconnect(this);   
    createAudioOutput();
}

void AudioTest::notified()
{
    qWarning() << "bytesFree = " << m_audioOutput->bytesFree()
               << ", " << "elapsedUSecs = " << m_audioOutput->elapsedUSecs()
               << ", " << "processedUSecs = " << m_audioOutput->processedUSecs();
}

void AudioTest::pullTimerExpired()
{
    if (m_audioOutput && m_audioOutput->state() != QAudio::StoppedState) {
        int chunks = m_audioOutput->bytesFree()/m_audioOutput->periodSize();
        while (chunks) {
           const qint64 len = m_generator->read(m_buffer.data(), m_audioOutput->periodSize());
           if (len)
               m_output->write(m_buffer.data(), len);
           if (len != m_audioOutput->periodSize())
               break;
           --chunks;
        }
    }
}

void AudioTest::toggleMode()
{
    m_pullTimer->stop();
    m_audioOutput->stop();

    if (m_pullMode) {
        m_output = m_audioOutput->start();
        m_pullMode = false;
        m_pullTimer->start(20);
    } else {
        m_pullMode = true;
        m_audioOutput->start(m_generator);
    }

}

void AudioTest::toggleSuspendResume()
{
    if (m_audioOutput->state() == QAudio::SuspendedState) {
        qWarning() << "status: Suspended, resume()";
        m_audioOutput->resume();
    } else if (m_audioOutput->state() == QAudio::ActiveState) {
        qWarning() << "status: Active, suspend()";
        m_audioOutput->suspend();
    } else if (m_audioOutput->state() == QAudio::StoppedState) {
        qWarning() << "status: Stopped, resume()";
        m_audioOutput->resume();
    } else if (m_audioOutput->state() == QAudio::IdleState) {
        qWarning() << "status: IdleState";
    }
}

void AudioTest::stateChanged(QAudio::State state)
{
    qWarning() << "state = " << state;
}
