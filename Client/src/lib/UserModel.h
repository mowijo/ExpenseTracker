#ifndef USERMODEL_H
#define USERMODEL_H

#include <QAbstractItemModel>

#include "Account.h"

class UserModel : public QAbstractItemModel
{

    Q_OBJECT

public:

    UserModel();
    ~UserModel();

    void setAccount(pAccount account);

    QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const ;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

public slots:
    void update();

signals:
    void errorOccured(class Exception *e);

private:
    friend class UserModelPrivate;
    class UserModelPrivate *d;
};

#endif // USERMODEL_H

