#include <QApplication>
#include <QObject>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QStringListModel>
#include <QIODevice>
#include <QHash>
#include <QPair>
#include <QTime>
#include <QDebug>
#include "audiooutput.h"

QStringListModel files;
QList<QPair<QStringList, CMBaseAudioSource *> * >decoders;
QHash<QRegExp *, CMBaseAudioSource *> regToDecoder;

bool scanDirectories(QStringList &paths, const QStringList &filter, QStringList &list)
{
    if (paths.isEmpty())
        return false;

    const QString path=paths.takeFirst();
    QDirIterator it(path, filter, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString f;
        QFileInfo info;

        f=it.next();

        info=it.fileInfo();
        if (info.isFile())
            list.append(f);
        else if (info.isDir()) {
            paths.append(f);
        }
    }

    return !paths.isEmpty();
}

void createDecoderRegExp()
{
    for (int i = 0; i < decoders.size(); i++) {
        QStringList filters=decoders.at(i)->first;
        for (int f = 0; f < filters.size(); f++) {
            QRegExp *r=new QRegExp(filters.at(f), Qt::CaseInsensitive, QRegExp::Wildcard);
            regToDecoder.insert(r, decoders.at(i)->second);
        }
    }
}

CMBaseAudioSource *findSuitableDecoder(QString file)
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

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    int r;
    bool mr;
    QStringList filter;
    QStringList fileList;
    QStringList directoryStack;
    AudioTest *at;
    CMBaseAudioSource *decoder;

    app.setApplicationName("Qt CM Player test");

    qsrand(QTime::currentTime().msec());

    filter << CMModPlugAudioSource::extensions();
    filter << CMSidAudioSource::extensions();
    filter << CMSapAudioSource::extensions();

    directoryStack << "/home/milang/Music";

    while (scanDirectories(directoryStack, filter, fileList)) {
        // XXX
    }

    at=new AudioTest();

    decoders.append(new QPair<QStringList, CMBaseAudioSource *>(CMModPlugAudioSource::extensions(), static_cast<CMBaseAudioSource*>(new CMModPlugAudioSource(at))));

    decoders.append(new QPair<QStringList, CMBaseAudioSource *>(CMSidAudioSource::extensions(), static_cast<CMBaseAudioSource*>(new CMSidAudioSource(at))));

    decoders.append(new QPair<QStringList, CMBaseAudioSource *>(CMSapAudioSource::extensions(), static_cast<CMBaseAudioSource*>(new CMSapAudioSource(at))));

    qDebug() << decoders;

    createDecoderRegExp();

    QString rfile=fileList.at(qrand() % fileList.size());

    qDebug() << "Playing file: " << rfile;

    decoder=findSuitableDecoder(rfile);
    mr=at->load(rfile, decoder);

    if (mr) {
        at->play();
        r=app.exec();
    } else {
        r=255;
    }

    delete at;

    return r;
}
