#include "cmmediascanner.h"
#include <QDebug>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>

#define TABLE_MEDIA_FILES "CREATE TABLE IF NOT EXISTS mediafiles (\
path text not null,\
title text,\
meta text,\
type varchar,\
rating int not null default -1,\
primary key (path));"

CMMediaScanner::CMMediaScanner(QObject *parent) : QObject(parent)
{
    m_ticker.setSingleShot(true);
    m_ticker.setInterval(10);
    connect(&m_ticker, SIGNAL(timeout()), this, SLOT(scanLoop()));

    m_tables.insert("mediafiles", TABLE_MEDIA_FILES);
    //m_tables.insert("playhistory", TABLE_MEDIA_FILES);
    //m_tables.insert("files", TABLE_MEDIA_FILES);
}

CMMediaScanner::~CMMediaScanner()
{
    m_db.close();
}

void CMMediaScanner::setFilters(const QStringList &filters)
{
    m_filter=filters;
}

bool CMMediaScanner::createTable(const QString table)
{
    QSqlQuery query(m_db);
    bool r;
    QString tdef=m_tables.value(table);

    if (tdef.isEmpty()) {
        qWarning() << "Definition not found for table " << table;
        return false;
    }

    r=query.exec(tdef);
    if (!r)
        qWarning("Table query failed");
    return r;
}

bool CMMediaScanner::initialize(const QString &db)
{
    m_db=QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(db);

    m_db_ok=m_db.open();
    if (!m_db_ok) {
        qWarning("Failed to open database");
        return false;
    }

    createTable("mediafiles");

    m_model=new CMLibraryModel(this);

    return m_db_ok;
}

uint CMMediaScanner::count()
{
    QSqlQuery q(m_db);
    uint c=0;

    if (!q.exec("SELECT COUNT(*) AS c FROM mediafiles")) {
        qWarning("Media count query failed");
        return -1;
    }
    if (q.isActive() && q.first())
        c=q.value(0).toUInt();
    else
        c=-1;

    qDebug() << "COUNT" << c;

    return c;
}

CMLibraryModel *CMMediaScanner::model()
{    
    return m_model;
}

void CMMediaScanner::addFilter(const QString &filter)
{
    m_filter << filter;
}

bool CMMediaScanner::addPath(const QString &path)
{
    if (m_paths.contains(path)) {
        qWarning() << "Scan path already exist in scan list";
        return true;
    }

    QDir p(path);
    if (p.makeAbsolute()==false) {
        qWarning() << "Scan path is invalid, failed to create absolute path";
        return false;
    }

    if (p.exists(path)==false) {
        qWarning() << "Scan path does not exist";
        return false;
    }

    m_paths.append(path);
    qDebug() << path;

    return true;
}

void CMMediaScanner::clearFilters()
{
    m_filter.clear();
}

void CMMediaScanner::clearPaths()
{
    m_paths.clear();
}

bool CMMediaScanner::scanAsync()
{
    if (m_paths.isEmpty())
        return false;

    if (scan(true))
        m_ticker.start();

    return true;
}

bool CMMediaScanner::scanAsyncCancel()
{
    if (m_ticker.isActive()) {
        m_ticker.stop();
        emit scanningDone();
        m_model->refresh();
        return true;
    }

    return false;
}

bool CMMediaScanner::addFile(const QString &file)
{
    QFile f(file);
    QFileInfo fi(file);

    if (!f.exists())
        return false;

    QSqlQuery query(m_db);

    query.prepare("INSERT OR REPLACE INTO mediafiles (path, title, type) VALUES (?, ?, -1)");
    query.bindValue(0, file);
    query.bindValue(1, fi.baseName());

    if (!query.exec()) {
        qWarning() << "Query failed: " << query.lastError() ;
        qDebug() << query.lastQuery();
        return false;
    }

    return true;
}

void CMMediaScanner::scanLoop()
{
    if (scan(false))
        m_ticker.start();
    else
        m_model->refresh();
}

/**
 * @brief CMMediaScanner::scanDirectories
 * @param paths
 * @param filter
 * @param list
 * @return
 */
bool CMMediaScanner::scan(bool fromStart)
{
    bool r;

    if (m_paths.isEmpty())
        return false;

    if (m_pathsleft.isEmpty() || fromStart)
        m_pathsleft=m_paths;

    const QString path=m_pathsleft.takeFirst();
    QDirIterator it(path, m_filter, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);

    emit scanning(path);
    m_db.transaction();
    while (it.hasNext()) {
        QString f;
        QFileInfo info;

        f=it.next();       
        info=it.fileInfo();
        if (info.isFile()) {
            addFile(f);            
        } else if (info.isDir()) {
            m_pathsleft.append(f);            
        }
    }
    m_db.commit();
    emit scannedPath(path);

    r=m_pathsleft.isEmpty();
    if (r)
        emit scanningDone();

    return !r;
}


