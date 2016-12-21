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
#include "cmmediascanner.h"
#include "cmmediadecoder.h"

QStringListModel files;

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    int r;
    bool mr;    
    QStringList fileList;    
    AudioTest *at;
    CMBaseAudioSource *decoder;
    CMMediaScanner scanner;
    CMMediaDecoder mdec;

    app.setApplicationName("Qt CM Player test");

    qsrand(QTime::currentTime().msec());

    scanner.setFilters(mdec.getSupportedExtensions());
    scanner.addPath("/home/milang/Music");

    while (scanner.scan(fileList)) {
        // XXX
    }

    at=new AudioTest();

    QString rfile=fileList.at(qrand() % fileList.size());

    qDebug() << "Playing file: " << rfile;

    decoder=mdec.findSuitableDecoder(rfile);
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
