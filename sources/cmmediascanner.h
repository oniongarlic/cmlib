#ifndef CMMEDIASCANNER_H
#define CMMEDIASCANNER_H

#include <QObject>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QStringList>

class CMMediaScanner : public QObject
{
    Q_OBJECT
public:
    explicit CMMediaScanner(QObject *parent = 0);
    Q_INVOKABLE void setFilters(const QStringList &filter);
    Q_INVOKABLE bool addPath(const QString path);
    Q_INVOKABLE void clearPaths();
    Q_INVOKABLE bool scan(QStringList &list);

signals:
    void scanning(QString path);
    void scanningDone();

public slots:

private:
    QStringList m_pathsleft;
    QStringList m_paths;
    QStringList m_filter;
};

#endif // CMMEDIASCANNER_H
