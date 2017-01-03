#include "cmwavfileaudiosink.h"

#include <QDebug>

CMWavFileAudioSink::CMWavFileAudioSink(QObject *parent)
    : CMFileAudioSink(parent)
{
    sample_rate=44100;
    channels=2;
    bps=2;
    byte_rate = sample_rate*channels*bps;
}

static void appendle(unsigned int word, int num_bytes, QByteArray &data)
{
    char buf;

    while(num_bytes>0) {
        buf = word & 0xff;
        data.append(buf);
        num_bytes--;
        word >>= 8;
    }
}

bool CMWavFileAudioSink::afterOpen()
{
    QByteArray header;

    qDebug() << "Writing WAV header";

    header.append("RIFF", 4);

    appendle(0, 4, header); // Dummy size until close

    header.append("WAVE", 4);
    header.append("fmt ", 4);

    appendle(16, 4, header); // Chunk size
    appendle(1, 2, header); // PCM
    appendle(channels, 2, header); // Channels
    appendle(sample_rate, 4, header); // Rate
    appendle(byte_rate, 4, header); //
    appendle(channels * bps, 2, header); //
    appendle(8*bps, 2, header); //

    header.append("data", 4);
    appendle(0, 4, header);

    qint64 w=m_file.write(header);

    if (w==-1)
        return false;

    return true;
}

bool CMWavFileAudioSink::aboutToClose()
{
    QByteArray tmp;

    qint64 s=m_file.size();
    if (s>INT_MAX)
        s=INT_MAX;

    qDebug() << "Writing WAV header sample size";

    appendle(s-8, 4, tmp);

    m_file.seek(4);
    m_file.write(tmp);

    tmp.clear();
    appendle(s-36, 4, tmp);

    m_file.seek(40);
    m_file.write(tmp);

    return true;
}
