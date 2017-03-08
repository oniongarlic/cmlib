#include "cmmediascanner.h"
#include <QDebug>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>

#define TABLE_MEDIA_FILES "CREATE TABLE IF NOT EXISTS mediafiles (\
path text not null,\
title text,\
meta text,\
type int not null,\
rating int not null default -1,\
primary key (path));"

CMMediaScanner::CMMediaScanner(QObject *parent) : QObject(parent)
{
    m_ticker.setSingleShot(true);
    m_ticker.setInterval(100);
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

bool CMMediaScanner::initialize(QString db)
{
    m_db=QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(db);

    m_db_ok=m_db.open();
    if (!m_db_ok) {
        qWarning("Failed to open database");
    } else {
        createTable("mediafiles");
    }

    return m_db_ok;
}

void CMMediaScanner::addFilter(const QString &filter)
{
    m_filter << filter;
}

bool CMMediaScanner::addPath(const QString path)
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

    if (scan(m_filelist, true))
        m_ticker.start();

    return true;
}

void CMMediaScanner::scanLoop()
{
    if (scan(m_filelist, false))
        m_ticker.start();
}

/**
 * @brief CMMediaScanner::scanDirectories
 * @param paths
 * @param filter
 * @param list
 * @return
 */
bool CMMediaScanner::scan(QStringList &list, bool fromStart)
{
    bool r;

    if (m_paths.isEmpty())
        return false;

    if (m_pathsleft.isEmpty() || fromStart)
        m_pathsleft=m_paths;

    const QString path=m_pathsleft.takeFirst();
    QDirIterator it(path, m_filter, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);

    emit scanning(path);

    while (it.hasNext()) {
        QString f;
        QFileInfo info;

        f=it.next();        

        info=it.fileInfo();
        if (info.isFile()) {
            QSqlQuery query(m_db);

            list.append(f);

            query.prepare("INSERT INTO mediafiles (path, type) VALUES (?, -1)");
            query.bindValue(0, f);

            if (!query.exec()) {
                qWarning() << "Query failed: " << query.lastError() ;
                qDebug() << query.lastQuery();
            }
        } else if (info.isDir()) {
            m_pathsleft.append(f);            
        }
    }

    r=!m_pathsleft.isEmpty();
    if (r)
        emit scanningDone();

    return r;
}


