#include "cmlibrarymodel.h"

#include <QFileInfo>
#include <QDebug>
#include <QModelIndex>

CMLibraryModel::CMLibraryModel(QObject *parent)
    : QSqlQueryModel(parent)
{    
#if QT_VERSION < 0x050000
    setRoleNames(roleNames());
#endif
    refresh();
}

QVariant CMLibraryModel::data(const QModelIndex &index, int role) const
{    
    QVariant value = QSqlQueryModel::data(index, role);

    if(role < Qt::UserRole)
        return value;

    int columnIdx = role - Qt::UserRole - 1;
    QModelIndex modelIndex = this->index(index.row(), columnIdx);
    value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);

    switch (role) {
    case FileNameRole:
    {
        QFileInfo f(value.toString());
        value=f.fileName();
    }
        break;    
    default:
        break;
    }

    qDebug() << role << value;

    return value;
}

void CMLibraryModel::refresh()
{
    setQuery("SELECT path,type,title,meta FROM mediafiles ORDER BY title");
}

uint CMLibraryModel::count() const
{
    return rowCount(QModelIndex());
}

QVariantMap CMLibraryModel::get(int index) const
{
    QVariantMap map;

    map.insert("file", QSqlQueryModel::data(this->index(index, 0), Qt::DisplayRole));
    map.insert("type", QSqlQueryModel::data(this->index(index, 1), Qt::DisplayRole));
    map.insert("title", QSqlQueryModel::data(this->index(index, 2), Qt::DisplayRole));

    return map;
}

