#ifndef CUTEMEDIAMODEL_H
#define CUTEMEDIAMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QStringList>
#include <QVariantMap>
#include <QSqlDatabase>

class CMLibraryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CMLibraryModel(QObject *parent = 0);

    enum Roles {FileRole = Qt::UserRole, FileNameRole, TypeRole, TitleRole };

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE uint count() const;
    Q_INVOKABLE void clear();
    Q_INVOKABLE QVariantMap get(int index) const;

    void setStringList(QStringList *list);

    QHash<int, QByteArray> roleNames() const {
        QHash<int, QByteArray> roles;
        roles[FileRole] = "file";
        roles[TypeRole] = "type";
        roles[TitleRole] = "title";
        roles[FileNameRole] = "filename";
        return roles;
    }

private:    
    QStringList *m_data;
    QStringList m_dummy;
};

#endif // CUTEMEDIAMODEL_H
