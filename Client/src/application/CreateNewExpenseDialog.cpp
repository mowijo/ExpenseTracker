#include "CreateNewExpenseDialog.h"

#include <QDebug>
#include <QMessageBox>

#include <Expense.h>

#include "ui_CreateNewExpenseDialog.h"



class CreateNewExpenseDialogPrivate : public QObject
{
    Q_OBJECT

private:
    CreateNewExpenseDialog *owningdialog;

public:

    Ui::CreateNewExpenseDialog *ui;
    pAccount account;
    QString cannot_create_expense;

    CreateNewExpenseDialogPrivate(CreateNewExpenseDialog *owner) :
        QObject(owner),
        owningdialog(owner),
        ui(new Ui::CreateNewExpenseDialog)
    {
        ui->setupUi(owner);
        cannot_create_expense = tr("Can Not Create Expense");
    }

public slots:

    void createNewExpense()
    {
        int amount = ui->amount_edit->value()*100;
        QDateTime date = ui->date_edit->dateTime().toUTC();
        QString description = ui->description_edit->text().trimmed();
        QString comment = ui->comment_edit->toPlainText();
        QString error = "";
        if(description == "")
        {
            ui->description_edit->setFocus();
            error += "<li>"+tr("You need to provide a description.")+"</li>";
        }
        if(amount == 0)
        {
            error += "<li>"+tr("You need to provide an amount greater than zero.")+"</li>";
            ui->amount_edit->setFocus();
        }

        if ( error != "" )
        {
            QMessageBox::information(owningdialog,
                                     cannot_create_expense,
                                     tr("The expense cannot be created because:")+"<ul>" + error + "</ul>"
                                     );
            return;
        }
        try
        {
            int id = Expense::doCreate(account, amount, date, description, comment);
            emit owningdialog->expenseCreated(id);
            clear();
            if( ui->keep_dialog_open_checkbox->isChecked() )
            {
                ui->amount_edit->setFocus();
                ui->date_edit->setDateTime(date);
            }
            else
            {
                owningdialog->close();
            }

        }
        catch(int error)
        {
            qDebug() << "Could not create due to error" << error;
            clear();
            owningdialog->close();
        }

    }

    void clear()
    {
        ui->amount_edit->setValue(0);
        ui->date_edit->setDateTime(QDateTime::currentDateTime());
        ui->comment_edit->setText("");
        ui->description_edit->setText("");
    }
};

#include "CreateNewExpenseDialog.moc"

CreateNewExpenseDialog::CreateNewExpenseDialog(QWidget *parent) :
    QDialog(parent),
    d(new CreateNewExpenseDialogPrivate(this))
{

    connect(d->ui->create_button, &QPushButton::clicked, [=]()
    {
        d->createNewExpense();
    });
}

CreateNewExpenseDialog::~CreateNewExpenseDialog()
{
    delete d->ui;
    delete d;
}

void CreateNewExpenseDialog::setAccount(pAccount account)
{
    d->account = account;
}
