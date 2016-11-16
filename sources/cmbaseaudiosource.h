#ifndef CMBASEAUDIOSOURCE_H
#define CMBASEAUDIOSOURCE_H

#include <QObject>
#include <QVariantHash>
#include <QIODevice>

class CMBaseAudioSource : public QIODevice
{
    Q_OBJECT
    Q_PROPERTY(quint8 channels READ channels WRITE setChannels NOTIFY channelsChanged)
    Q_PROPERTY(quint32 rate READ rate WRITE setRate NOTIFY rateChanged)
    Q_PROPERTY(quint16 track READ track WRITE setTrack NOTIFY trackChanged)
    Q_PROPERTY(quint16 tracks READ tracks WRITE setTracks NOTIFY tracksChanged)
    Q_PROPERTY(QVariantHash meta READ meta NOTIFY metaChanged)
    Q_PROPERTY(bool valid READ valid NOTIFY validChanged)
    Q_PROPERTY(quint64 position READ position WRITE setPosition NOTIFY positionChanged)

public:
    CMBaseAudioSource(QObject *parent);

    // QIODevice interface
public:
    qint64 bytesAvailable() const;
    bool canReadLine() const;

    quint8 channels() const
    {
        return m_channels;
    }

    quint32 rate() const
    {
        return m_rate;
    }

    quint16 track() const
    {
        return m_track;
    }

    quint16 tracks() const
    {
        return m_tracks;
    }

    QVariantHash meta() const
    {
        return m_meta;
    }

    bool valid() const
    {
        return m_valid;
    }

    quint64 position() const
    {
        return m_position;
    }

public slots:
    void setChannels(quint8 channels);
    void setRate(quint32 rate);
    virtual void setTrack(quint16 track);
    void setTracks(quint16 tracks);    
    void setPosition(quint64 position);

signals:
    void channelsChanged(quint8 channels);
    void rateChanged(quint32 rate);
    void trackChanged(quint16 track);
    void tracksChanged(quint16 tracks);
    void metaChanged(QVariantHash meta);
    void validChanged(bool valid);

    void positionChanged(quint64 position);

protected slots:
    void setvalid(bool valid);

protected:
    virtual bool generateData(qint64 maxlen) = 0;
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);    

    QByteArray m_buffer;
    qint64 m_pos;

    // Available tracks and current track. For source that support many internal tracks (for example SID files)
    quint16 m_track;
    quint16 m_tracks;

    quint8 m_channels;
    quint32 m_rate;

    // Stream length and curren position. If length is unknown then it is 0
    quint64 m_length;
    quint64 m_position;

    // Meta data (title, author, etc)
    QVariantHash m_meta;

    bool m_valid;
};

#endif // CMBASEAUDIOSOURCE_H
