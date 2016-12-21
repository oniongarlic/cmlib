#ifndef CMMEDIASCANNER_H
#define CMMEDIASCANNER_H

#include <QObject>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QTimer>

class CMMediaScanner : public QObject
{
    Q_OBJECT
public:
    explicit CMMediaScanner(QObject *parent = 0);
    Q_INVOKABLE void setFilters(const QStringList &filter);
    Q_INVOKABLE bool addPath(const QString path);
    Q_INVOKABLE void clearPaths();
    Q_INVOKABLE bool scan(QStringList &list, bool fromStart=false);
    Q_INVOKABLE bool scanAsync();

signals:
    void scanning(QString path);
    void scanningDone();

public slots:

private slots:
    void scanLoop();

private:
    QStringList m_pathsleft;
    QStringList m_paths;
    QStringList m_filter;
    QTimer m_ticker;
    QStringList m_filelist;
};

#endif // CMMEDIASCANNER_H
