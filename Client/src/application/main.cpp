#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

#include <Exception.h>

#include "LoginDialog.h"
#include "MainWindow.h"


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    LoginDialog logindialog;
    MainWindow mainwindow;
    logindialog.show();

    QObject::connect(&logindialog, &LoginDialog::loginFailed, [&]()
    {
        QMessageBox::warning(0,
                             QObject::tr("Login Failed"),
                             QObject::tr("The email address and password you provided could not be matched."));
        logindialog.show();
    });


    QObject::connect(&logindialog, &LoginDialog::userCreationFailed, [&](const QString &reason)
    {
        QMessageBox::warning(0,
                             QObject::tr("Login Failed"),
                             reason);
        logindialog.show();
    });

    QObject::connect(&logindialog, &LoginDialog::loggedInWith, [&](pAccount a)
    {
        mainwindow.setAccount(a);
        mainwindow.show();
    });


    QObject::connect(&logindialog, &LoginDialog::userCreated, [&](pAccount a)
    {
        QMessageBox::information(0,
                                 QObject::tr("Account Created"),
                                 QObject::tr(
                                     "An account is created for <b>%1</b> with password <b>%2</b> and that user "
                                     "will now be logged in.\n\nIn a real application, this would be "
                                     "done diffrently with some mailing of credentials and account"
                                     " activation, but not now. Not today.").arg(a->username()).arg(a->password()));
        mainwindow.setAccount(a);
        mainwindow.show();
    });

    QObject::connect(&logindialog, &LoginDialog::loginCancelled, [&]()
    {
        QCoreApplication::quit();
    });

      return app.exec();

}
