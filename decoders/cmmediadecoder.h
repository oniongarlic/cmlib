#ifndef CMMEDIADECODER_H
#define CMMEDIADECODER_H

#include <QObject>
#include <QSet>

#include "cmbaseaudiosource.h"

#ifdef SID_DECODER
#include "cmsidaudiosource.h"
#endif

#ifdef MOD_DECODER
#include "cmmodplugaudiosource.h"
#endif

#ifdef MTP_DECODER
#include "cmopenmtpaudiosource.h"
#endif

#ifdef SAP_DECODER
#include "cmsapaudiosource.h"
#endif

#ifdef SC68_DECODER
#include "cmsc68audiosource.h"
#endif

#ifdef AY_DECODER
#include "cmayaudiosource.h"
#endif

#ifdef FLAC_DECODER
#include "cmflacaudiosource.h"
#endif

#ifdef OPUS_DECODER
#include "cmopusaudiosource.h"
#endif

class CMMediaDecoder : public QObject
{
    Q_OBJECT
public:
    explicit CMMediaDecoder(QObject *parent = 0);
    CMBaseAudioSource *findSuitableDecoder(QString file);
    const QStringList getSupportedExtensions() const;

signals:
    void metadata(QVariantHash meta);
    void eot();

protected slots:
    void decoderMetadata(QVariantHash meta);
    void decoderEOT();

protected:
    void analyzeDecoders();
private:    
    QHash<QRegExp *, CMBaseAudioSource *> regToDecoder;
    QHash<QString, QString> m_ext_to_mime;
    QList<CMBaseAudioSource *> m_decoders;
    QStringList m_extensions;
};

#endif // CMMEDIADECODER_H
