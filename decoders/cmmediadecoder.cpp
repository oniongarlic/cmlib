#include "cmmediadecoder.h"

#include <QDebug>

CMMediaDecoder::CMMediaDecoder(QObject *parent) : QObject(parent)
{
    // Registers our known source decoders
    decoders.append(new QPair<QStringList, CMBaseAudioSource *>(CMModPlugAudioSource::extensions(), static_cast<CMBaseAudioSource*>(new CMModPlugAudioSource(this))));
    decoders.append(new QPair<QStringList, CMBaseAudioSource *>(CMSidAudioSource::extensions(), static_cast<CMBaseAudioSource*>(new CMSidAudioSource(this))));
    decoders.append(new QPair<QStringList, CMBaseAudioSource *>(CMSapAudioSource::extensions(), static_cast<CMBaseAudioSource*>(new CMSapAudioSource(this))));

#ifdef FLAC_DECODER
    decoders.append(new QPair<QStringList, CMBaseAudioSource *>(CMFlacAudioSource::extensions(), static_cast<CMBaseAudioSource*>(new CMFlacAudioSource(this))));
#endif

    createDecoderRegExp();
    connectDecoderSignals();
}

void CMMediaDecoder::connectDecoderSignals()
{
    for (int i = 0; i < decoders.size(); i++) {
        CMBaseAudioSource *d=decoders.at(i)->second;

        connect(d, SIGNAL(metaChanged(QVariantHash)), this, SLOT(decoderMetadata(QVariantHash)));
    }
}

void CMMediaDecoder::createDecoderRegExp()
{
    for (int i = 0; i < decoders.size(); i++) {
        QStringList filters=decoders.at(i)->first;

        for (int f = 0; f < filters.size(); f++) {
            QRegExp *r=new QRegExp(filters.at(f), Qt::CaseInsensitive, QRegExp::Wildcard);
            regToDecoder.insert(r, decoders.at(i)->second);
        }
    }
}

QStringList CMMediaDecoder::getSupportedExtensions()
{
    QStringList e;

#if 0
    for (int i = 0; i < decoders.size(); i++) {
        CMBaseAudioSource *d=decoders.at(i)->second;

        e << d->extensions();
    }
#else
    e << CMModPlugAudioSource::extensions();
    e << CMSidAudioSource::extensions();
    e << CMSapAudioSource::extensions();
#ifdef FLAC_DECODER
    e << CMFlacAudioSource::extensions();
#endif
#endif
    return e;
}

void CMMediaDecoder::decoderMetadata(QVariantHash meta)
{
    qDebug() << "Got decoder meta data " << meta;
    emit metadata(meta);
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
