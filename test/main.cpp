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

#ifdef QTAUDIO
#include "cmqtaudiosink.h"
#endif

#ifdef ALSAAUDIO
#include "cmalsaaudiosink.h"
#endif

#ifdef QNXAUDIO
#include "cmqsaaudiosink.h"
#endif

#include "cmfileaudiosink.h"
#include "cmwavfileaudiosink.h"

#include "cmmediaplayer.h"
#include "cmmediascanner.h"
#include "decoders/cmmediadecoder.h"

QStringListModel files;

void myMessageOutput(QtMsgType type, const char* msg){
               fprintf(stdout, "%s\n", msg);
               fflush(stdout);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStringList fileList;
    CMMediaScanner scanner;
    CMMediaDecoder mdec;
    int r;

    qInstallMsgHandler(myMessageOutput);

    app.setApplicationName("Qt CM Player test");

    QDeclarativeView viewer;
    QDeclarativeContext *context = viewer.rootContext();

    viewer.connect(viewer.engine(), SIGNAL(quit()), SLOT(close()));
    viewer.setResizeMode(QDeclarativeView::SizeRootObjectToView);

    qmlRegisterType<CMMediaPlayer>("org.tal.cm", 1, 0, "CMMediaPlayer");
    qmlRegisterUncreatableType<CMBaseAudioSink>("org.tal.cm", 1, 0, "CMBaseAudioSink", "CMBaseAudioSink is abstract and can not be created");

#ifdef QTAUDIO
    qmlRegisterType<CMQtAudioSink>("org.tal.cm", 1, 0, "CMAudioSink");
#endif
#ifdef ALSAAUDIO
    qmlRegisterType<CMAlsaAudioSink>("org.tal.cm", 1, 0, "CMAudioSink");
#endif
#ifdef QNXAUDIO
    qmlRegisterType<CMQSAAudioSink>("org.tal.cm", 1, 0, "CMAudioSink");
#endif
    // qmlRegisterType<CMFileAudioSink>("org.tal.cm", 1, 0, "CMFileAudioSink");
    qmlRegisterType<CMWavFileAudioSink>("org.tal.cm", 1, 0, "CMWavFileAudioSink");

    qsrand(QTime::currentTime().msec());

    scanner.setFilters(mdec.getSupportedExtensions());
    // scanner.addPath(QDesktopServices::storageLocation(QDesktopServices::MusicLocation));

    scanner.addPath("/home/milang/Music");
#if defined(Q_OS_BLACKBERRY)
    scanner.addPath("/accounts/1000/shared/music");
#endif

    while (scanner.scan(fileList)) {
        // XXX
    }

    fileList.sort();

    context->setContextProperty("_files", &files);    
    //context->setContextProperty("_scanner", &scanner);

#if defined(Q_OS_BLACKBERRY)
    viewer.setSource(QUrl::fromLocalFile("app/native/test/main.qml"));
#else
    viewer.setSource(QUrl::fromLocalFile("test/main.qml"));
#endif
    files.setStringList(fileList);

    // QString rfile=fileList.at(qrand() % fileList.size());

#if defined(Q_OS_QNX)
    viewer.showFullScreen();
#else
    viewer.show();
#endif

    return app.exec();

    return r;
}
