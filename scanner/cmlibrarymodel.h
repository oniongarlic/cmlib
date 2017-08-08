#ifndef CUTEMEDIAMODEL_H
#define CUTEMEDIAMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QStringList>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlQueryModel>

class CMLibraryModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    CMLibraryModel(QObject *parent = 0);

    enum Roles {FileRole = Qt::UserRole, FileNameRole, TypeRole, TitleRole };

    QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE uint count() const;
    Q_INVOKABLE QVariantMap get(int index) const;

    QHash<int, QByteArray> roleNames() const {
        QHash<int, QByteArray> roles;
        roles[FileRole] = "file";
        roles[TypeRole] = "type";
        roles[TitleRole] = "title";
        roles[FileNameRole] = "filename";
        return roles;
    }

private:    
    QSqlDatabase m_db;
};

#endif // CUTEMEDIAMODEL_H
