#include "cmmediadecoder.h"

#include <QDebug>
#include <QMetaObject>
#include <QMetaClassInfo>

#define ADD_SOURCE(_src) { decoders.append(new QPair<QStringList, CMBaseAudioSource *>(_src::extensions(), static_cast<CMBaseAudioSource*>(new _src(this)))); }

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
    for (int i = 0; i < decoders.size(); i++) {
        const QMetaObject *mo;
        QMetaClassInfo mci;
        QStringList filters=decoders.at(i)->first;
        CMBaseAudioSource *d=decoders.at(i)->second;

        for (int f = 0; f < filters.size(); f++) {
            QRegExp *r=new QRegExp(filters.at(f), Qt::CaseInsensitive, QRegExp::Wildcard);
            regToDecoder.insert(r, decoders.at(i)->second);
        }

        mo=d->metaObject();
        mci=mo->classInfo(mo->indexOfClassInfo("mime"));

        qDebug() << mo->className() << mci.value();

        connect(d, SIGNAL(metaChanged(QVariantHash)), this, SLOT(decoderMetadata(QVariantHash)));
        connect(d, SIGNAL(eot()), this, SLOT(decoderEOT()));
    }
}

QStringList CMMediaDecoder::getSupportedExtensions()
{
    QStringList e;

    for (int i = 0; i < decoders.size(); i++)
        e << decoders.at(i)->first;;

    return e;
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
