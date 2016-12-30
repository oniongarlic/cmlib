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
    CMMediaPlayer player;
    CMQtAudioSink sink;
    int r;

    app.setApplicationName("Qt CM Player test");

    QDeclarativeView viewer;
    QDeclarativeContext *context = viewer.rootContext();

    viewer.connect(viewer.engine(), SIGNAL(quit()), SLOT(close()));
    viewer.setResizeMode(QDeclarativeView::SizeRootObjectToView);

    //viewer.addImportPath(QLatin1String("modules"));

    qsrand(QTime::currentTime().msec());

    player.setAudioSink(&sink);

    scanner.setFilters(mdec.getSupportedExtensions());
    // scanner.addPath(QDesktopServices::storageLocation(QDesktopServices::MusicLocation));
    scanner.addPath("/home/milang/Music/modules");
    scanner.addPath("/home/milang/Music/sidmusic");

    while (scanner.scan(fileList)) {
        // XXX
    }

    fileList.sort();

    context->setContextProperty("_files", &files);
    context->setContextProperty("_player", &player);
    //context->setContextProperty("_scanner", &scanner);

    viewer.setSource(QUrl::fromLocalFile("test/main.qml"));
    files.setStringList(fileList);

    // QString rfile=fileList.at(qrand() % fileList.size());

    viewer.show();
    return app.exec();

    return r;
}
