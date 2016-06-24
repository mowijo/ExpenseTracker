#ifndef CREATENEWUSERDIALOG_H
#define CREATENEWUSERDIALOG_H

#include <QDialog>

#include <Account.h>

class CreateNewUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateNewUserDialog(QWidget *parent = 0);
    ~CreateNewUserDialog();
    void setAccount(pAccount account);

signals:
    void newUserCreated();

private:
    class CreateNewUserDialogPrivate *d;
};

#endif // CREATENEWUSERDIALOG_H
