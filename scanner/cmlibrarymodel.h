#ifndef CUTEMEDIAMODEL_H
#define CUTEMEDIAMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVariantMap>

class CMLibraryModel : public QAbstractListModel
{
public:
    CMLibraryModel(QObject *parent = Q_NULLPTR);

    enum Roles {FileRole = Qt::UserRole, FileNameRole, TitleRole };

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool insertRows(int row, int count, const QModelIndex &parent);

    Q_INVOKABLE void clear();
    Q_INVOKABLE QVariantMap get(int index) const;

    QHash<int, QByteArray> roleNames() const {
        QHash<int, QByteArray> roles;
        roles[FileRole] = "file";
        roles[TitleRole] = "title";
        roles[FileNameRole] = "filename";
        return roles;
    }

private:

};

#endif // CUTEMEDIAMODEL_H
