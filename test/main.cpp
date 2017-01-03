/**
  * A simple QtQuick 1.1 UI test application for CMlib
  * GPL v2
  *
  */
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

#include <QDesktopServices>

#include "cmqtaudiosink.h"
#include "cmfileaudiosink.h"
#include "cmwavfileaudiosink.h"
#include "cmmediaplayer.h"
#include "cmmediascanner.h"
#include "decoders/cmmediadecoder.h"

QStringListModel files;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStringList fileList;
    CMMediaScanner scanner;
    CMMediaDecoder mdec;
    int r;

    app.setApplicationName("Qt CM Player test");

    QDeclarativeView viewer;
    QDeclarativeContext *context = viewer.rootContext();

    viewer.connect(viewer.engine(), SIGNAL(quit()), SLOT(close()));
    viewer.setResizeMode(QDeclarativeView::SizeRootObjectToView);

    qmlRegisterType<CMMediaPlayer>("org.tal.cm", 1, 0, "CMMediaPlayer");
    qmlRegisterUncreatableType<CMBaseAudioSink>("org.tal.cm", 1, 0, "CMBaseAudioSink", "CMBaseAudioSink is abstract and can not be created");
    qmlRegisterType<CMQtAudioSink>("org.tal.cm", 1, 0, "CMQtAudioSink");
    qmlRegisterType<CMFileAudioSink>("org.tal.cm", 1, 0, "CMFileAudioSink");
    qmlRegisterType<CMWavFileAudioSink>("org.tal.cm", 1, 0, "CMWavFileAudioSink");

    qsrand(QTime::currentTime().msec());

    scanner.setFilters(mdec.getSupportedExtensions());
    // scanner.addPath(QDesktopServices::storageLocation(QDesktopServices::MusicLocation));
    scanner.addPath("/home/milang/CMTestFiles");
    scanner.addPath("/home/milang/Music/modules");
    scanner.addPath("/home/milang/Music/sidmusic");

    while (scanner.scan(fileList)) {
        // XXX
    }

    fileList.sort();

    context->setContextProperty("_files", &files);    
    //context->setContextProperty("_scanner", &scanner);

    viewer.setSource(QUrl::fromLocalFile("test/main.qml"));
    files.setStringList(fileList);

    // QString rfile=fileList.at(qrand() % fileList.size());

    viewer.show();
    return app.exec();

    return r;
}
