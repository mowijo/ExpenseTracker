#include "UserModel.h"

#include <QDebug>

#include "Exception.h"
#include "User.h"

class UserModelPrivate
{
    UserModel *owner;

public:

    pAccount account;
    QList<pUser> users;
    UserModelPrivate(UserModel *o)
        : owner(o)
    {
    }

};

UserModel::UserModel()
{
    d = new UserModelPrivate(this);
}


UserModel::~UserModel()
{
    delete d;
}

void UserModel::setAccount(pAccount account)
{
    d->account = account;
    update();
}

QModelIndex UserModel::index(int row, int column, const QModelIndex &) const
{
    if(row < 0 || row >= d->users.length() || column < 0 || column >= columnCount())
    {
        return QModelIndex();
    }

    return createIndex(row, column, (void*)(d->users[row].get()));
}

QModelIndex UserModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}


int UserModel::rowCount(const QModelIndex &) const
{
    return d->users.length();
}

int UserModel::columnCount(const QModelIndex &) const
{
    return 4;
}

QVariant UserModel::data(const QModelIndex &index, int role) const
{
    User *u = static_cast<User*>(index.internalPointer());
    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
        case 0: return u->name();
        case 1: return u->email();
        case 2: return QVariant();
        case 3: return QVariant();
        }
    }

    if(role == Qt::CheckStateRole)
    {
        if(index.column() == 2)
        {
            return u->mayApproveExpenses() ? Qt::Checked : Qt::Unchecked;
        }
        if(index.column() == 3)
        {
            return u->mayManageUsers() ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QVariant();
}

QVariant UserModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical) return QVariant();
    if(role == Qt::DisplayRole)
    {
        switch(section)
        {
        case 0: return tr("Name");
        case 1: return tr("Email");
        case 2: return tr("Approve Expenses");
        case 3: return tr("Manage Users");
        default: break;
        }
    }
    return QVariant();
}

Qt::ItemFlags UserModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = Qt::ItemIsEnabled;
    if(index.column() == 2 || index.column() == 3)
    {
        f |= Qt::ItemIsUserCheckable ;
    }
    if(index.column() == 0 || index.column() == 1)
    {
        f |= Qt::ItemIsEditable;
    }
    return f;
}

bool UserModel::setData(const QModelIndex &index, const QVariant &value, int )
{
    User *u = static_cast<User*>(index.internalPointer());
    if(index.column() == 0)
    {
        u->setName(value.toString());
    }
    if(index.column() == 1)
    {
        u->setEmail(value.toString());
    }
    if(index.column() == 2)
    {
        u->setMayApproveExpenses(value.toBool());
    }
    if(index.column() == 3)
    {
        u->setMayManageUsers(value.toBool());
    }

    try
    {
        u->doSave(d->account);
        return true;
    }
    catch(Exception *e)
    {
        emit errorOccured(e);
        update();
        delete e;
    }
    return false;
}

void UserModel::update()
{
    if( d->account.get() )
    {
        beginResetModel();
        d->users = User::doFetchAll(d->account);
        endResetModel();
    }
}
