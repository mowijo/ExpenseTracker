#ifndef EXPENSEMODEL_H
#define EXPENSEMODEL_H


#include <QAbstractItemModel>
#include <QStyledItemDelegate>

#include "Account.h"

class ExpenseModel : public QAbstractItemModel
{

    Q_OBJECT

public:

    ExpenseModel();
    ~ExpenseModel();

    void setAccount(pAccount account);
    pAccount account() const;
    void setApprovalEnabled(bool may_approve);

    QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const ;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QStyledItemDelegate* editorForColumn(int column);

    QString reportAsHtml() const;

public slots:
    void update();

signals:
    void errorOccured(class Exception *e);
    void errorOccured(const QString &title, const QString &message);

private:
    friend class ExpenseModelPrivate;
    class ExpenseModelPrivate *d;
};


#endif // EXPENSEMODEL_H

