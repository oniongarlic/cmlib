#include "cmmediascanner.h"
#include <QDebug>
#include <QDir>

CMMediaScanner::CMMediaScanner(QObject *parent) : QObject(parent)
{
    m_ticker.setSingleShot(true);
    m_ticker.setInterval(100);
    connect(&m_ticker, SIGNAL(timeout()), this, SLOT(scanLoop()));
}

void CMMediaScanner::setFilters(const QStringList &filters)
{
    m_filter=filters;
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
            list.append(f);            
        } else if (info.isDir()) {
            m_pathsleft.append(f);            
        }
    }

    r=!m_pathsleft.isEmpty();
    if (r)
        emit scanningDone();

    return r;
}


