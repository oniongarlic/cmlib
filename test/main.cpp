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

#include <QDeclarativeComponent>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeView>

#include "audiooutput.h"
#include "cmmediascanner.h"
#include "cmmediadecoder.h"

QStringListModel files;

void loadUI()
{

}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    int r;
    bool mr;
    QStringList fileList;
    AudioTest *at;
    CMBaseAudioSource *decoder;
    CMMediaScanner scanner;
    CMMediaDecoder mdec;

    app.setApplicationName("Qt CM Player test");

    QDeclarativeView viewer;
    QDeclarativeContext *context = viewer.rootContext();

    viewer.connect(viewer.engine(), SIGNAL(quit()), SLOT(close()));
    viewer.setResizeMode(QDeclarativeView::SizeRootObjectToView);

    //viewer.addImportPath(QLatin1String("modules"));

    qsrand(QTime::currentTime().msec());

    scanner.setFilters(mdec.getSupportedExtensions());
    scanner.addPath("/home/milang/Music");

    scanner.scan(fileList, true);
    while (scanner.scan(fileList)) {
        // XXX
    }

    // fileList.sort();

    at=new AudioTest();

    context->setContextProperty("_files", &files);
    context->setContextProperty("_player", at);

    viewer.setSource(QUrl::fromLocalFile("main.qml"));

    QString rfile=fileList.at(qrand() % fileList.size());

    files.setStringList(fileList);

    qDebug() << "Loading file: " << rfile;

    decoder=mdec.findSuitableDecoder(rfile);
    mr=at->load(rfile, decoder);

    if (mr) {
        // at->play();
        viewer.show();
        r=app.exec();
    } else {
        r=255;
    }

    delete at;

    return r;
}
