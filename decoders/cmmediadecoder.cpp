#include "cmmediadecoder.h"

#include <QDebug>
#include <QMetaObject>
#include <QMetaClassInfo>

#define ADD_SOURCE(_src) { m_decoders.append(static_cast<CMBaseAudioSource*>(new _src(this))); }

CMMediaDecoder::CMMediaDecoder(QObject *parent) : QObject(parent)
{
    // Registers our known source decoders
    qDebug("DECODER: Registering sources");

#ifdef MOD_DECODER
    ADD_SOURCE(CMModPlugAudioSource)
#endif
#ifdef MTP_DECODER
    ADD_SOURCE(CMOpenMTPAudioSource)
#endif
#ifdef SID_DECODER
    ADD_SOURCE(CMSidAudioSource)
#endif
#ifdef SAP_DECODER
    ADD_SOURCE(CMSapAudioSource)
#endif
#ifdef SC68_DECODER
    ADD_SOURCE(CMSC68AudioSource)
#endif
#ifdef AY_DECODER
    ADD_SOURCE(CMAYAudioSource)
#endif
#ifdef FLAC_DECODER
    ADD_SOURCE(CMFlacAudioSource)
#endif
#ifdef OPUS_DECODER
    ADD_SOURCE(CMOpusAudioSource)
#endif

    analyzeDecoders();
}

void CMMediaDecoder::analyzeDecoders()
{
    for (int i = 0; i < m_decoders.size(); i++) {
        CMBaseAudioSource *src=m_decoders.at(i);
        const QMetaObject *mo;
        QMetaClassInfo mci_mime, mci_extensions;
        QStringList ext;

        mo=src->metaObject();
        mci_mime=mo->classInfo(mo->indexOfClassInfo("mime"));
        mci_extensions=mo->classInfo(mo->indexOfClassInfo("extensions"));

        qDebug() << "Decoder Info: " << mo->className() << mci_mime.value() << mci_extensions.value();

        QString tmp(mci_extensions.value());
        ext=tmp.split(";");

        qDebug() << ext;

        m_extensions << ext;

        for (int f = 0; f < ext.size(); f++) {
            QRegExp *r=new QRegExp(ext.at(f), Qt::CaseInsensitive, QRegExp::Wildcard);
            regToDecoder.insert(r, src);
        }

        connect(src, SIGNAL(metaChanged(QVariantHash)), this, SLOT(decoderMetadata(QVariantHash)));
        connect(src, SIGNAL(eot()), this, SLOT(decoderEOT()));
    }
}

const QStringList CMMediaDecoder::getSupportedExtensions() const
{
    qDebug() << m_extensions;
    return m_extensions;
}

void CMMediaDecoder::decoderMetadata(QVariantHash meta)
{
    qDebug() << "Got decoder meta data " << meta;
    emit metadata(meta);
}

void CMMediaDecoder::decoderEOT()
{
    qDebug() << "Decoder EoT";
    emit eot();
}

/**
 * @brief CMMediaDecoder::findSuitableDecoder
 * @param file
 * @return
 */
CMBaseAudioSource *CMMediaDecoder::findSuitableDecoder(QString file)
{
    QHashIterator<QRegExp *, CMBaseAudioSource *> i(regToDecoder);
    while (i.hasNext()) {
        i.next();
        QRegExp *r=i.key();
        if (r->exactMatch(file))
            return i.value();
    }

    qWarning() << "Failed to find suitable decoder for file: " << file;

    return NULL;
}
