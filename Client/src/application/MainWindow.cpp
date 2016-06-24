#include "MainWindow.h"

#include <QDebug>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QTimer>
#include <QTextBrowser>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

#include <Exception.h>
#include <Expense.h>
#include <ExpenseModel.h>
#include <User.h>
#include <UserModel.h>

#include "CreateNewUserDialog.h"
#include "CreateNewExpenseDialog.h"
#include "ui_MainWindow.h"


class MainWindowPrivate : public QObject
{

    Q_OBJECT

public:
    Ui::MainWindow *ui;
    UserModel usermodel;
    ExpenseModel expensesmodel;
    CreateNewUserDialog newuserdialog;
    CreateNewExpenseDialog newexpensedialog;

    QTextBrowser tb;
    QPrintPreviewDialog dialog;

    bool mayapprove_expenses;

    MainWindowPrivate()
        :ui(new Ui::MainWindow)
    {
    }


    void setUserManagementEnabled(bool enabled)
    {
        ui->action_create_new_user->setEnabled(enabled);
        ui->users_view->setEnabled(enabled);
        ui->tabWidget->setTabEnabled(1, enabled);
        ui->menu_Users->setEnabled(enabled);

    }

    void disableUserManagement()
    {
        setUserManagementEnabled(false);
        ui->tabWidget->setCurrentIndex(0);
    }

    void enableUserManagement()
    {
        setUserManagementEnabled(true);
        ui->tabWidget->setCurrentIndex(1);
    }

public slots:

    void printReport()
    {
        tb.setHtml( expensesmodel.reportAsHtml());
        QObject::connect(&dialog, &QPrintPreviewDialog::paintRequested, [&](QPrinter *printer){
            tb.print(printer);
        });
        dialog.exec();
    }

    void applyFilters()
    {
        QDate from;
        QDate to;
        QString search;
        bool show_approved;
        bool show_pending;

        if(ui->filter_from_box->isChecked())
        {
            from = ui->filter_from->date();
        }
        if(ui->filter_to_box->isChecked())
        {
            to = ui->filter_to->date();
        }
        if(ui->filter_approved_status->currentIndex() == 0)
        {
            show_approved = true;
            show_pending = true;
        }
        else if(ui->filter_approved_status->currentIndex() == 1)
        {
            show_approved = true;
            show_pending = false;
        }
        else
        {
            show_approved = false;
            show_pending = true;
        }

        search = ui->filter_text->text();
        ui->expense_view->setFilters(from, to, search, show_approved, show_pending);
    }
};


#include "MainWindow.moc"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d(new MainWindowPrivate())
{
    d->ui->setupUi(this);
    d->ui->users_view->setModel(&d->usermodel);
    d->ui->expense_view->setModel(&d->expensesmodel);
    d->ui->tabWidget->setCurrentIndex(1);


    connect(&d->newexpensedialog, &CreateNewExpenseDialog::expenseCreated, &d->expensesmodel, &ExpenseModel::update);
    connect(&d->newuserdialog, &CreateNewUserDialog::newUserCreated, &d->usermodel, &UserModel::update);


    //Connect events from view to the logic hanling it.
    connect(d->ui->users_view, &QWidget::customContextMenuRequested, [=](const QPoint & pos)
    {
        QModelIndex index = d->ui->users_view->indexAt(pos);
        if( ! index.isValid() )
        {
            return;
        }
        if(index.column() != 0)
        {
            return;
        }

        User *user = static_cast<User*>(index.internalPointer());

        QMenu menu;
        QAction deleteAction(tr("Delete %1").arg(user->name()), this);
        menu.addAction(&deleteAction);

        connect(&deleteAction, &QAction::triggered, [=](){
            try
            {
                //d->usermodel.deleteUser(user->email());
            }
            catch(int httpcode)
            {

            }
        });

        menu.exec(d->ui->users_view->mapToGlobal(pos));
    });

    connect(d->ui->expense_view, &ExpenseView::numberOfSelectedExpensesChanged, [=](int amount)
    {
        //We will update the menu entries so they reflect what is possible with the
        //selected expenses.
        if ( d->mayapprove_expenses )
        {
            d->ui->action_Delete_Expense->setEnabled(false);

            if(amount == 0)
            {
                d->ui->action_Approve_Expenses->setEnabled(false);
                d->ui->action_Approve_Expenses->setText(tr("Approve expenses"));
            }
            else
            {
                d->ui->action_Approve_Expenses->setEnabled(true);
                d->ui->action_Approve_Expenses->setText(tr("Approve %n expense(s)", "", amount).arg(amount));
            }
        }
        else
        {
            d->ui->action_Approve_Expenses->setEnabled(false);
            if(amount == 0)
            {
                d->ui->action_Delete_Expense->setEnabled(false);
                d->ui->action_Delete_Expense->setText(tr("Delete expenses"));
            }
            else
            {
                d->ui->action_Delete_Expense->setEnabled(true);
                d->ui->action_Delete_Expense->setText(tr("Delete %n expense(s)", "", amount).arg(amount));
            }
        }
    });



    //Connect error from data models to displaying them
    connect(&d->usermodel, &UserModel::errorOccured, [=](Exception *e)
    {
        QMessageBox::critical(this,
                              e->headLine(),
                              e->body()+"<p>"+e->remedy());
    });
    connect(&d->expensesmodel,
             static_cast<void (ExpenseModel::*)(const QString&,const QString&)>(&ExpenseModel::errorOccured),
            [=](QString headline,QString body)
    {
        QMessageBox::critical(this,
                              headline,
                              body);
    });


    //Setup and configure the actions in menus.
    connect(d->ui->action_Print_Report, &QAction::triggered, d, &MainWindowPrivate::printReport);
    connect(d->ui->action_Delete_Expense, &QAction::triggered, d->ui->expense_view, &ExpenseView::deleteSelection);
    connect(d->ui->action_create_new_user, &QAction::triggered, &d->newuserdialog, &QDialog::show);
    connect(d->ui->action_create_expense,  &QAction::triggered, &d->newexpensedialog, &QDialog::show);
    connect(d->ui->actionUpdate_Model, &QAction::triggered, &d->usermodel, &UserModel::update);
    connect(d->ui->actionUpdate_Model, &QAction::triggered, &d->expensesmodel, &ExpenseModel::update);




    //Configure and setup widgets used to filter the expense list.
    d->ui->filter_approved_status->addItems(QStringList() << tr("Both") << tr("Approved only") << tr("Pending only"));
    d->ui->filter_to->setDisplayFormat("d. MMM. yyyy");
    d->ui->filter_from->setDisplayFormat("d. MMM. yyyy");

    connect(d->ui->filter_approved_status, SIGNAL(currentIndexChanged(int)), d, SLOT(applyFilters()));
    connect(d->ui->filter_from, SIGNAL(dateChanged(QDate)), d, SLOT(applyFilters()));
    connect(d->ui->filter_from_box, SIGNAL(toggled(bool)), d, SLOT(applyFilters()));
    connect(d->ui->filter_to, SIGNAL(dateChanged(QDate)), d, SLOT(applyFilters()));
    connect(d->ui->filter_to_box, SIGNAL(toggled(bool)), d, SLOT(applyFilters()));
    connect(d->ui->filter_text, SIGNAL(textChanged(QString)), d, SLOT(applyFilters()));


}

MainWindow::~MainWindow()
{
    delete d->ui;
    delete d;
}

void MainWindow::setAccount(pAccount account)
{
    this->setWindowTitle(tr("Expense Tracking for %1").arg(account->username()));

    pUser user = User::doLoad(account->username(), account);
    if( user->mayManageUsers() )
    {

        d->enableUserManagement();
        d->usermodel.setAccount(account);
        d->newuserdialog.setAccount(account);
        d->ui->menu_Expenses->setEnabled(false);
        d->ui->tabWidget->setTabEnabled(0, false);
        d->ui->action_create_expense->setEnabled(false);
    }
    else
    {
        d->disableUserManagement();

        d->mayapprove_expenses = user->mayApproveExpenses();
        d->ui->action_create_expense->setEnabled( ! user->mayApproveExpenses() );
    }

    d->expensesmodel.setAccount(account);
    d->expensesmodel.setApprovalEnabled(user->mayApproveExpenses());
    d->newexpensedialog.setAccount(account);

}
