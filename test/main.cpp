/**
  * A simple QtQuick 1.1 UI test application for CMlib
  * GPL v2
  *
  */

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

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>
#include <QtQml>
#else
#include <QApplication>
#include <QDeclarativeComponent>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeView>
#endif

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
#include "scanner/cmmediascanner.h"
#include "scanner/cmlibrarymodel.h"
#include "decoders/cmmediadecoder.h"

#if 0
void myMessageOutput(QtMsgType type, const char* msg)
{
    fprintf(stdout, "%s\n", msg);
    fflush(stdout);
}
#endif

int main(int argc, char **argv)
{
#if QT_VERSION >= 0x050000
    QGuiApplication app(argc, argv);
#else
     QApplication app(argc, argv);
#endif   
    CMMediaScanner scanner;
    CMMediaDecoder mdec;

    app.setApplicationName("Qt CM Player test");

    qsrand(QTime::currentTime().msec());

    //qInstallMsgHandler(myMessageOutput);

    qmlRegisterType<CMMediaPlayer>("org.tal.cm", 1, 0, "CMMediaPlayer");
    qmlRegisterType<CMMediaScanner>("org.tal.cm", 1, 0, "CMMediaScanner");
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

    qmlRegisterUncreatableType<CMLibraryModel>("org.tal.cm", 1, 0, "MediaLibraryModel", "C++ only");
    //qRegisterMetaType<CMLibraryModel *>();

    scanner.initialize("mediadatabase.db");
    scanner.setFilters(mdec.getSupportedExtensions());    

#if QT_VERSION < 0x050000
    scanner.addPath(QDesktopServices::storageLocation(QDesktopServices::MusicLocation));
#else
    scanner.addPath(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
#endif

#if defined(Q_OS_BLACKBERRY)
    scanner.addPath("/accounts/1000/shared/music");
#else
    scanner.addPath("/home/milang/Music");
    scanner.addPath("/home/milang/CMTestFiles");
#endif

    if (scanner.count()==0) {
        scanner.scanAsync();
    } else {
        scanner.model()->refresh();
    }    
    qDebug() << "Model count" << scanner.model()->count();

#if QT_VERSION < 0x050000
    QDeclarativeView viewer;
    QDeclarativeContext *context = viewer.rootContext();

    viewer.connect(viewer.engine(), SIGNAL(quit()), SLOT(close()));
    viewer.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    context->setContextProperty("_files", scanner.model());
    context->setContextProperty("_scanner", &scanner);
#if defined(Q_OS_BLACKBERRY)
    viewer.setSource(QUrl::fromLocalFile("qrc:/qtquick1-main.qml"));
#else
    viewer.setSource(QUrl(QString("qrc:/qtquick1-main.qml")));
#endif
#if defined(Q_OS_QNX)
    viewer.showFullScreen();
#else
    viewer.show();
#endif
#else
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("_files", scanner.model());
    engine.rootContext()->setContextProperty("_scanner", &scanner);
    engine.load(QUrl(QStringLiteral("qrc:/qtquick2-main.qml")));
#endif

    return app.exec();
}
