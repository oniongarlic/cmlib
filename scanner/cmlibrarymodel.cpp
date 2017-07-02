#include "cmlibrarymodel.h"

#include <QFileInfo>
#include <QDebug>

CMLibraryModel::CMLibraryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_data=&m_dummy;

#if QT_VERSION < 0x050000
    setRoleNames(roleNames());
#endif
}

int CMLibraryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return count();
}

QVariant CMLibraryModel::data(const QModelIndex &index, int role) const
{    
    if (!index.isValid()) {
        qWarning("Invalid index");
        return QVariant();
    }

    if (m_data->count()<index.row()) {
        qWarning("Index out of bounds");
        return QVariant();
    }

    const QString f=m_data->value(index.row());
    QFileInfo fi(f);

    switch (role) {
    case CMLibraryModel::FileRole:
        return QVariant(f);
        break;
    case CMLibraryModel::TitleRole:
        return QVariant(fi.baseName()); // XXX
        break;
    case CMLibraryModel::TypeRole:
        return QVariant(fi.suffix().toLower());
        break;
    case CMLibraryModel::FileNameRole:
        return QVariant(fi.baseName());
        break;
    }

    return QVariant();
}

uint CMLibraryModel::count() const
{
    return m_data->size();
}

void CMLibraryModel::clear()
{
    beginResetModel();
    m_data->clear();
    endResetModel();
}

QVariantMap CMLibraryModel::get(int index) const
{
    QVariantMap map;
    const QString f=m_data->value(index);
    QFileInfo fi(f);

    map.insert("file", m_data->value(index));
    map.insert("filename", fi.baseName());

    map.insert("type", "");
    map.insert("title", "");

    return map;
}

void CMLibraryModel::setStringList(QStringList *list)
{
    qDebug("setStringList");
    beginResetModel();
    m_data=list;
    endResetModel();
}
