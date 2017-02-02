#include "cmlibrarymodel.h"

CMLibraryModel::CMLibraryModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

int CMLibraryModel::rowCount(const QModelIndex &parent) const
{

}

QVariant CMLibraryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        qWarning("Invalid index");
        return QVariant();
    }

    // XXX

    switch (role) {
    case CMLibraryModel::FileRole:
        return QVariant();
        break;
    case CMLibraryModel::TitleRole:
        return QVariant();
        break;
    case CMLibraryModel::FileNameRole:
        return QVariant();
        break;
    }

    return QVariant();
}

bool CMLibraryModel::insertRows(int row, int count, const QModelIndex &parent)
{

}

void CMLibraryModel::clear()
{
    beginResetModel();
    // m_data.clear();
    endResetModel();
}

QVariantMap CMLibraryModel::get(int index) const
{
    QVariantMap map;

    return map;
}
