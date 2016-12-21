#include "cmmediascanner.h"
#include <QDebug>
#include <QDir>

CMMediaScanner::CMMediaScanner(QObject *parent) : QObject(parent)
{

}

void CMMediaScanner::setFilters(const QStringList &filter)
{
    m_filter=filter;
}

bool CMMediaScanner::addPath(const QString path)
{
    if (m_paths.contains(path))
        return true;

    QDir p(path);
    if (p.makeAbsolute()==false)
        return false;

    if (p.exists(path)==false)
        return false;

    m_paths.append(path);
    qDebug() << path;

    return true;
}

void CMMediaScanner::clearPaths()
{
    m_paths.clear();
}

/**
 * @brief CMMediaScanner::scanDirectories
 * @param paths
 * @param filter
 * @param list
 * @return
 */
bool CMMediaScanner::scan(QStringList &list)
{
    bool r;

    if (m_paths.isEmpty())
        return false;

    if (m_pathsleft.isEmpty())
        m_pathsleft=m_paths;

    const QString path=m_pathsleft.takeFirst();
    QDirIterator it(path, m_filter, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    emit scanning(path);

    while (it.hasNext()) {
        QString f;
        QFileInfo info;

        f=it.next();

        info=it.fileInfo();
        if (info.isFile())
            list.append(f);
        else if (info.isDir()) {
            m_pathsleft.append(f);
        }
    }

    r=!m_pathsleft.isEmpty();
    if (r)
        emit scanningDone();

    return r;
}
