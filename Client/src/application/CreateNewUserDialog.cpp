#include "CreateNewUserDialog.h"

#include <QDebug>
#include <QMessageBox>

#include <Exception.h>
#include <User.h>

#include "ui_CreateNewUserDialog.h"
class CreateNewUserDialogPrivate : public QObject
{

    Q_OBJECT

private:
    CreateNewUserDialog *owner;

public:

    CreateNewUserDialogPrivate(CreateNewUserDialog *owner):
        owner(owner),
        ui(new Ui::CreateNewUserDialog)
    {
    }

    pAccount account;
    Ui::CreateNewUserDialog *ui;

public slots:
    void createNewUser()
    {
        if(ui->name_edig->text().trimmed() == "")
        {
            QMessageBox::information(owner, tr("Will not create user"), tr("You need to specify the name of the new user"));
            return;
        }
        if(ui->email_edit->text().trimmed() == "")
        {
            QMessageBox::information(owner, tr("Will not create user"), tr("You need to specify the email for the new user"));
            return;
        }
        QString password;
        try
        {
            password = User::doCreate(ui->email_edit->text().trimmed(), ui->name_edig->text(), account);
            pUser newuser = User::doLoad(ui->email_edit->text().trimmed(), account);
            newuser->setMayApproveExpenses(ui->may_approve_expenses_checkbox->isChecked());
            newuser->setMayManageUsers(ui->may_manage_users_checkbox->isChecked());
            newuser->doSave(account);
            clear();
            if( ! ui->keep_dialog_open_checkbox->isChecked() )
            {
                owner->close();
            }
            emit owner->newUserCreated();
        }
        catch(int errorcode)
        {
            switch(errorcode)
            {
                case 409:
                    QMessageBox::information(owner,
                            tr("Could not Create User"),
                            tr("The user could not be  created because the email address <b>%1</b> is already in use by another user.").arg(ui->email_edit->text().trimmed())
                            +"<br>"
                            +tr("Select another email address or modify or remove the user aldready using %1").arg(ui->email_edit->text().trimmed()));

            }

        }


    }

    void clear()
    {
        ui->name_edig->setText("");
        ui->email_edit->setText("");
        ui->may_approve_expenses_checkbox->setChecked(false);
        ui->may_manage_users_checkbox->setCheckable(false);
    }
};


#include "CreateNewUserDialog.moc"

CreateNewUserDialog::CreateNewUserDialog(QWidget *parent) :
    QDialog(parent),
    d(new CreateNewUserDialogPrivate(this))
{
    d->ui->setupUi(this);

    connect(d->ui->create_button, &QPushButton::clicked, d, &CreateNewUserDialogPrivate::createNewUser);

    connect(d->ui->cancel_button, &QPushButton::clicked, this, &QDialog::reject);
    connect(d->ui->cancel_button, &QPushButton::clicked, d, &CreateNewUserDialogPrivate::clear);
}

CreateNewUserDialog::~CreateNewUserDialog()
{
    delete d->ui;
    delete d;
}

void CreateNewUserDialog::setAccount(pAccount account)
{
    d->account = account;
}
