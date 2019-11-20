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
    CMLibraryModel(QObject *parent = nullptr);

    enum Roles {FileRole = Qt::UserRole, FileNameRole, TypeRole, TitleRole, MetaRole };

    QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE int count() const;
    Q_INVOKABLE QVariantMap get(int index) const;

    Q_INVOKABLE void search(const QString &str);

    QHash<int, QByteArray> roleNames() const {
        QHash<int, QByteArray> roles;
        roles[FileRole] = "file";
        roles[TypeRole] = "type";
        roles[TitleRole] = "title";
        roles[FileNameRole] = "filename";
        roles[MetaRole] = "metadata";
        return roles;
    }

private:    
    QSqlDatabase m_db;
    QString m_search;
};

#endif // CUTEMEDIAMODEL_H
