#ifndef CMFLACAUDIOSOURCE_H
#define CMFLACAUDIOSOURCE_H

#include <QObject>
#include <QBuffer>

#include "cmbaseaudiosource.h"

#include <FLAC++/decoder.h>

class CMFlacAudioSource : public CMBaseAudioSource, FLAC::Decoder::Stream
{
    Q_OBJECT
    Q_CLASSINFO("mime", "audio/x-flac")
    Q_CLASSINFO("extensions", "*.flac")
public:
    CMFlacAudioSource(QObject *parent=0);
    ~CMFlacAudioSource();

    // Stream interface
protected:
    FLAC__StreamDecoderReadStatus read_callback(FLAC__byte buffer[], size_t *bytes);
    FLAC__StreamDecoderWriteStatus write_callback(const FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
    void error_callback(FLAC__StreamDecoderErrorStatus status);

    // CMBaseAudioSource interface
protected:
    qint64 generateData(qint64 maxlen);

    // QIODevice interface
public:
    bool open(OpenMode mode);
    void close();
    bool reset();

    // Stream interface
    qint64 readData(char *data, qint64 maxlen);
protected:
    void metadata_callback(const FLAC__StreamMetadata *metadata);

private:
    int m_bps;
    float m_track_pos;
    int m_sample_rate;
    quint64 m_total_samples;
    QBuffer m_read_buffer;
    bool m_eot;

    // Stream interface
protected:
    bool eof_callback();
};

#endif // CMFLACAUDIOSOURCE_H
