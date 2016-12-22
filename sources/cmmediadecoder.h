#ifndef CMMEDIADECODER_H
#define CMMEDIADECODER_H

#include <QObject>

#include "cmbaseaudiosource.h"
#include "cmsidaudiosource.h"
#include "cmmodplugaudiosource.h"
#include "cmsapaudiosource.h"

class CMMediaDecoder : public QObject
{
    Q_OBJECT
public:
    explicit CMMediaDecoder(QObject *parent = 0);
    CMBaseAudioSource *findSuitableDecoder(QString file);
    QStringList getSupportedExtensions();

signals:
    void metadata(QVariantHash meta);

public slots:
    void decoderMetadata(QVariantHash meta);

protected:
    void createDecoderRegExp();
private:
    QList<QPair<QStringList, CMBaseAudioSource *> * >decoders;
    QHash<QRegExp *, CMBaseAudioSource *> regToDecoder;
    void connectDecoderSignals();
};

#endif // CMMEDIADECODER_H