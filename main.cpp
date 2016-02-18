#include <QApplication>
#include <QObject>
#include <QIODevice>
#include <QDebug>
#include "audiooutput.h"

int main(int argc, char **argv)
{
QApplication app(argc, argv);

app.setApplicationName("Qt SID Player");

AudioTest *at;
at=new AudioTest();

return app.exec();
}
