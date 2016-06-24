#ifndef CREATENEWEXPENSEDIALOG_H
#define CREATENEWEXPENSEDIALOG_H

#include <QDialog>
#include <Account.h>

class CreateNewExpenseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateNewExpenseDialog(QWidget *parent = 0);
    ~CreateNewExpenseDialog();
    void setAccount(pAccount account);


signals:

    void expenseCreated(int id);

private:
    class CreateNewExpenseDialogPrivate *d;
};

#endif // CREATENEWEXPENSEDIALOG_H
