#ifndef EXPENSE_H
#define EXPENSE_H

#include <memory>

#include <QDateTime>
#include <QString>

#include "Account.h"

typedef std::shared_ptr<class Expense> pExpense;


class Expense
{

public:

    ~Expense();

    int id() const;
    int owner() const;
    int amount() const;
    bool approved() const;
    bool deleted();
    QDateTime epoch() const;
    QDateTime touched() const;
    QString description() const;
    QString comment() const;
    QString decoration() const;

    void setOwner(int owner) ;
    void setAmount(int amount) ;
    void setEpoch(const QDateTime &epoch) ;
    void setTouched(const QDateTime &touched) ;
    void setDescription(const QString &description);
    void setComment(const QString &comment);
    void setDecoration(const QString &decoration);

    void doSave(pAccount account);
    void doDelete(pAccount account);
    void doApprove(pAccount account);
    void doUnApprove(pAccount account);

    static int doCreate(pAccount account,
                            int amount,
                            const QDateTime &epoch,
                            const QString &description,
                            const QString &comment = ""
                            );

    static pExpense doLoad(int id, pAccount account);
    static QList<pExpense> doFetchAll(pAccount account);



private:
    Expense(int id);

    friend class ExpensePrivate;
    class ExpensePrivate *d;
    Expense(const Expense &other);
    Expense(const Expense &&other);
    Expense& operator=(Expense &rhs);
};

#endif // EXPENSE_H

