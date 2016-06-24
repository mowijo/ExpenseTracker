#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <Account.h>

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

signals:
    void loggedInWith(pAccount account);
    void loginCancelled();
    void loginFailed();
    void userCreationFailed(const QString &reason);
    void userCreated(pAccount account);

private:
    class LoginDialogPrivate *d;
};

#endif // LOGINDIALOG_H
