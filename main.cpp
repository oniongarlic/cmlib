#include <QApplication>
#include <QObject>
#include <QDirIterator>
#include <QFile>
#include <QStringListModel>
#include <QIODevice>
#include <QDebug>
#include "audiooutput.h"

QStringListModel files;

void scan_dir(const QString &path)
{
    QStringList bob;
    bob << "*.sid";

    QDirIterator it(path, bob);
    while (it.hasNext()) {
        QFile file(it.next());
        if (file.open(QIODevice::ReadOnly)) {
            qDebug() << "Opened:" << file.fileName() << endl;
        }else {
            qDebug() << "Can't open " << file.fileName() << file.errorString() << endl;
        }
        if(it.hasNext()==false){
            bob.clear();
            qDebug() << "Finished";
        }
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    app.setApplicationName("Qt SID Player");

    scan_dir("/home/milang/Music/sidmusic");

    AudioTest *at;
    at=new AudioTest();

    return app.exec();
}
