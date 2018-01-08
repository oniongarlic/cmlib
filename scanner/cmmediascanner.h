#ifndef CMMEDIASCANNER_H
#define CMMEDIASCANNER_H

#include <QObject>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlQueryModel>

#include "cmlibrarymodel.h"

class CMMediaScanner : public QObject
{
    Q_OBJECT
public:
    explicit CMMediaScanner(QObject *parent = 0);
    ~CMMediaScanner();
    void setFilters(const QStringList &filters);

    Q_INVOKABLE bool initialize(const QString &db);

    Q_INVOKABLE void addFilter(const QString &filter);
    Q_INVOKABLE bool addPath(const QString &path);
    Q_INVOKABLE void clearFilters();
    Q_INVOKABLE void clearPaths();

    Q_INVOKABLE bool scan(bool fromStart=false);
    Q_INVOKABLE bool scanAsync();
    Q_INVOKABLE bool scanAsyncCancel();

    Q_INVOKABLE bool addFile(const QString &file);
    Q_INVOKABLE bool updateFile(const QString &file, const QString title);

    Q_INVOKABLE uint count();

    Q_INVOKABLE CMLibraryModel *model();    
signals:
    void scanning(QString path);
    void scannedPath(const QString path);
    void scanningDone();

public slots:

protected:
    bool createTable(const QString table);
private slots:
    void scanLoop();

private:
    QSqlDatabase m_db;
    CMLibraryModel *m_model;
    bool m_db_ok;
    QMap<QString, QString> m_tables;

    QTimer m_ticker;
    QStringList m_pathsleft;
    QStringList m_paths;
    QStringList m_filter;
};

#endif // CMMEDIASCANNER_H
