#include "ExpenseView.h"

#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

#include <Expense.h>
#include <ExpenseModel.h>

class FilterProxy : public QSortFilterProxyModel
{

    Q_OBJECT

private:
    QDate from;
    QDate to;
    QString text;
    bool show_approved;
    bool show_pending;


public:
    FilterProxy(QObject *parent = 0)
        : QSortFilterProxyModel(parent)
    {
    }

    void setFilters(const QDate &from, const QDate &to, QString text, bool show_approved, bool show_pending)
    {
        this->from = from;
        this->to = to;
        this->text = text;
        this->show_approved = show_approved;
        this->show_pending = show_pending;
        invalidate();
    }


    bool filterAcceptsRow(int source_row, const QModelIndex &) const
    {
        QModelIndex index = sourceModel()->index(source_row, 0);
        Expense *expense = static_cast<Expense*>(index.internalPointer());


        if ( expense->deleted() )
        {
            return false;
        }

        if( ! (
                (this->show_approved && expense->approved())
                ||
                (this->show_pending && !expense->approved())
              )
        )
        {
            return false;
        }

        if( ! from.isNull() && from.isValid())
        {
            if(expense->epoch().date() < from)
            {
                return false;
            }
        }

        if( ! to.isNull() && from.isValid())
        {
            if(expense->epoch().date() > to)
            {
                return false;
            }
        }


        if(text == "")
        {
            return true;
        }
        if( expense->decoration().contains(text))
        {
            return true;
        }

        if( expense->comment().contains(text))
        {
            return true;
        }

        if(expense->description().contains(text))
        {
            return true;
        }

        return false;
    }

};

#include "ExpenseView.moc"

// Class Implementation ----------------------------------------------------------------------

class ExpenseViewPrivate
{
public:
    FilterProxy proxy;

};

ExpenseView::ExpenseView(QWidget *parent)
    : QTableView(parent)
{
    d = new ExpenseViewPrivate;
}

ExpenseView::~ExpenseView()
{
    delete d;
}

bool ExpenseView::isEditing() const
{
    return this->state() == QAbstractItemView::EditingState;
}

void ExpenseView::setModel(QAbstractItemModel *model)
{
    d->proxy.setSourceModel(model);
    QTableView::setModel(&d->proxy);

    hideColumn(5);
    hideColumn(8);
    hideColumn(9);
    ExpenseModel *expensemodel = dynamic_cast<ExpenseModel*>(model);
    if(expensemodel)
    {
        this->setItemDelegateForColumn(1, expensemodel->editorForColumn(1));
    }
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(selectionModel(), &QItemSelectionModel::selectionChanged, [=](){
        emit numberOfSelectedExpensesChanged(selectionModel()->selectedRows().size());
    });
    setFilters(QDate(), QDate(), "", true, true);
}

void ExpenseView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete)
    {
        this->deleteSelection();
        event->accept();
    }
}

void ExpenseView::deleteSelection()
{
    ExpenseModel *expensemodel = dynamic_cast<ExpenseModel*>(d->proxy.sourceModel());
    if( ! expensemodel)
    {
        return;
    }
    QModelIndexList rows = selectionModel()->selectedRows();
    foreach(QModelIndex index, rows)
    {
        QModelIndex sourcenidex = d->proxy.mapToSource(index);
        Expense *e = static_cast<Expense*>(sourcenidex.internalPointer());
        e->doDelete(expensemodel->account());
    }
    expensemodel->update();
    emit numberOfSelectedExpensesChanged(selectionModel()->selectedRows().size());
}



void ExpenseView::approveSelection()
{
    ExpenseModel *expensemodel = dynamic_cast<ExpenseModel*>(d->proxy.sourceModel());
    if( ! expensemodel)
    {
        return;
    }
    QModelIndexList rows = selectionModel()->selectedRows();
    foreach(QModelIndex index, rows)
    {
        QModelIndex sourcenidex = d->proxy.mapToSource(index);
        Expense *e = static_cast<Expense*>(sourcenidex.internalPointer());
        e->doApprove(expensemodel->account());
    }
    expensemodel->update();
    emit numberOfSelectedExpensesChanged(selectionModel()->selectedRows().size());
}

void ExpenseView::setFilters(const QDate &from, const QDate &to, QString text, bool show_approved, bool show_pending)
{
    d->proxy.setFilters(from, to, text, show_approved, show_pending);
}
