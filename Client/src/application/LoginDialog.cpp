    #include "LoginDialog.h"

#include <QDebug>

#include <Exception.h>
#include <User.h>

#include "ui_LoginDialog.h"

class LoginDialogPrivate
{

public:
    Ui::LoginDialog* ui;

    LoginDialogPrivate()
    {
        ui = new Ui::LoginDialog;
    }
};

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent)
{
    d = new LoginDialogPrivate;
    d->ui->setupUi(this);
    d->ui->password_edit->setText("foobar");
    d->ui->email_edit->setText("foo@bar.com");


    connect(d->ui->buttonBox, &QDialogButtonBox::accepted, [=](){
        pAccount a(new Account);
        a->setHost(d->ui->host_edit->text());
        a->setPort(d->ui->port_edit->value());
        a->setUserName(d->ui->email_edit->text());
        a->setPassword(d->ui->password_edit->text());

        if( d->ui->create_account_checkbox->isChecked() )
        {
            if(d->ui->password_edit->text() == "")
            {
                emit userCreationFailed(tr("You must enter a name"));
                return;
            }
            if(d->ui->email_edit->text() == "")
            {
                emit userCreationFailed(tr("You must enter an email address"));
                return;
            }
            QString password;
            try
            {
                password = User::doCreate(d->ui->email_edit->text(), d->ui->password_edit->text(), a);
            }
            catch(Exception *)
            {
                emit userCreationFailed(
                            tr("Email address %1 is already in use. Chose another.")
                                .arg(d->ui->email_edit->text()));
                return;
            }
            a->setPassword(password);
            a->setUserName(d->ui->email_edit->text());
            emit userCreated(a);

        }
        else
        {
            try
            {
                User::doLoad(a->username(), a);
                emit loggedInWith(a);
            }
            catch(Exception *e)
            {
                emit loginFailed();
            }
        }
    });

    connect(d->ui->buttonBox, &QDialogButtonBox::rejected, [=](){
        emit loginCancelled();
    });

    connect(d->ui->create_account_checkbox, &QCheckBox::toggled, [=](bool on)
    {
        d->ui->password_edit->setEchoMode( on ? QLineEdit::Normal : QLineEdit::Password);
        d->ui->password_edit->setText("");
        d->ui->password_edit->setPlaceholderText( on ? tr("Your name") : tr("Password"));
    });

}

LoginDialog::~LoginDialog()
{
    delete d->ui;
    delete d;
}
