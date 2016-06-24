#include "ExpenseModel.h"

#include <QDebug>
#include <QDoubleSpinBox>

#include <Expense.h>
#include <ReportBuilder.h>

// This class is for editing the amount field.
class AmountEditorDelegate : public QStyledItemDelegate
{
    Q_OBJECT

private:
    ExpenseModel *model;

public:
    AmountEditorDelegate(ExpenseModel *model, QWidget *parent = 0) : QStyledItemDelegate(parent)
    {
        this->model= model;
    }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const Q_DECL_OVERRIDE
    {
        QDoubleSpinBox * spinbox = new QDoubleSpinBox(parent);
        Expense *e = static_cast<Expense*>(index.internalPointer());
        spinbox->setValue(e->amount()/100.0);
        spinbox->setMaximum(10000000);
        spinbox->setMinimum(0);
        spinbox->setSuffix(" SDR");
        return spinbox;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE
    {
        QDoubleSpinBox *spinbox = dynamic_cast<QDoubleSpinBox*>(editor);
        Expense *e = static_cast<Expense*>(index.internalPointer());
        spinbox->setValue(e->amount()/100.0);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *, const QModelIndex &index) const Q_DECL_OVERRIDE
    {
        QDoubleSpinBox *spinbox = dynamic_cast<QDoubleSpinBox*>(editor);
        if( (int)(spinbox->value()*100) > 0)
        {
            Expense *e = static_cast<Expense*>(index.internalPointer());
            e->setAmount(spinbox->value()*100);
            e->doSave(this->model->account());
        }
    }

private slots:
    void commitAndCloseEditor()
    {
        qDebug() << "Commits editor...";
    }
};

#include "ExpenseModel.moc"

class ExpenseModelPrivate
{
    ExpenseModel *owner;

public:


    pAccount account;
    QList<pExpense> expenses;
    bool may_approve_expenses;
    bool may_edit_expenses;

    ExpenseModelPrivate(ExpenseModel *o)
        : owner(o)
    {
        may_approve_expenses = false;
        may_edit_expenses = true;
    }
};

ExpenseModel::ExpenseModel()
{
    d = new ExpenseModelPrivate(this);
}


ExpenseModel::~ExpenseModel()
{
    delete d;
}

void ExpenseModel::setAccount(pAccount account)
{
    d->account = account;
    update();
}

pAccount ExpenseModel::account() const
{
    return d->account;
}

void ExpenseModel::setApprovalEnabled(bool may_approve)
{
    d->may_approve_expenses = may_approve;
    d->may_edit_expenses = !may_approve;
}

QModelIndex ExpenseModel::index(int row, int column, const QModelIndex &) const
{
    if(row < 0 || row >= d->expenses.length() || column < 0 || column >= columnCount())
    {
        return QModelIndex();
    }

    return createIndex(row, column, (void*)(d->expenses[row].get()));
}

QModelIndex ExpenseModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int ExpenseModel::rowCount(const QModelIndex &) const
{
    return d->expenses.length();
}

int ExpenseModel::columnCount(const QModelIndex &) const
{
    return 10;
}

QVariant ExpenseModel::data(const QModelIndex &index, int role) const
{
    Expense *u = static_cast<Expense*>(index.internalPointer());
    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
        case 0: return u->epoch().toLocalTime().toString("d. MMM. yyyy");
        case 1: return QString::number((double)(u->amount()/100.0))+" SDR";
        case 2: return u->description();
        case 3: return u->decoration();
        case 4: return QVariant(); //Is handled as a checkbox further down.
        case 5: return u->deleted();
        case 6: return u->comment();
        case 7: return u->touched().toLocalTime().toString("d. MMM. yyyy, hh:mm");
        case 8: return u->id();
        case 9: return u->owner();
        }
    }

    if(role == Qt::CheckStateRole)
    {
        if(index.column() == 4)
        {
            return u->approved() ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QVariant();
}

QVariant ExpenseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical) return QVariant();
    if(role == Qt::DisplayRole)
    {
        switch(section)
        {
        case 0: return tr("Date");
        case 1: return tr("Amount");
        case 2: return tr("Description");
        case 3: return tr("Creator");
        case 4: return tr("Approved");
        case 5: return tr("Deleted");
        case 6: return tr("Comment");
        case 7: return tr("Last Modified");
        case 8: return tr("Id");
        case 9: return tr("Owner");
        default: break;
        }
    }
    return QVariant();

}

Qt::ItemFlags ExpenseModel::flags(const QModelIndex &index) const
{
    Expense *e = static_cast<Expense*>(index.internalPointer());
    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if(index.column() == 4 )
    {
        f |= Qt::ItemIsUserCheckable ;
    }
    if((index.column() == 1)  && (d->may_edit_expenses) && ( ! e->approved()))
    {
        f |= Qt::ItemIsEditable;
    }
    if((index.column() == 2)  && (d->may_edit_expenses) && ( ! e->approved()))
    {
        f |= Qt::ItemIsEditable;
    }
    if((index.column() == 6)  && (d->may_edit_expenses) && ( ! e->approved()))
    {
        f |= Qt::ItemIsEditable;
    }
    return f;
}

bool ExpenseModel::setData(const QModelIndex &index, const QVariant &value, int )
{
    Expense *e = static_cast<Expense*>(index.internalPointer());
    if(index.column() == 4)
    {
        bool checked = value.toBool();
        if(d->may_approve_expenses)
        {
            if(checked)
            {
                e->doApprove(d->account);
            }
            else
            {
                e->doUnApprove(d->account);
            }
        }
    }
    if(index.column() == 2)
    {
        QString description = value.toString().trimmed();
        if(description == "")
        {
            emit errorOccured(tr("Changes Not Saved"), tr("You are not allowed to save an expense without a description"));
            return true;
        }
        e->setDescription(description);
        e->doSave(d->account);
    }
    if(index.column() == 6)
    {
        QString comment = value.toString().trimmed();
        e->setComment(comment);
        e->doSave(d->account);
    }

    return true;
}

QStyledItemDelegate *ExpenseModel::editorForColumn(int column)
{
    if(column == 1)
    {
        return new AmountEditorDelegate(this);
    }
    return 0;
}

QString ExpenseModel::reportAsHtml() const
{
    ReportBuilder reportbuilder;
    QMap<int, QMap<int, QList<pExpense>*> > weeks; //weeks[year(int)][week(int)] = QList<pExpenses>
    foreach(pExpense expense, d->expenses )
    {
        reportbuilder.addExpense(expense);
    }

    return reportbuilder.html();
/*
    "<html>"
    "<head>"
    "<style type=\"text/css\">"
    ".tg  {border-collapse:collapse;border-spacing:0;border-color:#aaa;}"
    ".tg td{font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:0px;overflow:hidden;word-break:normal;border-color:#aaa;color:#333;background-color:#fff;border-top-width:1px;border-bottom-width:1px;}"
    ".tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:0px;overflow:hidden;word-break:normal;border-color:#aaa;color:#fff;background-color:#f38630;border-top-width:1px;border-bottom-width:1px;}"
    ".tg .tg-j2zy{background-color:#FCFBE3;vertical-align:top}"
    ".tg .tg-yw4l{vertical-align:top}"
    "</style>"
    "</head>"
    "<body>"

            "<table class=\"tg\">"
            "  <tr>"
            "    <th class=\"tg-yw4l\">\zxc</th>"
            "    <th class=\"tg-yw4l\">\zxc\zxc</th>"
            "    <th class=\"tg-yw4l\">\z\zxc\zxc</th>"
            "  </tr>"
            "  <tr>"
            "    <td class=\"tg-j2zy\">\zxc\zxc</td>"
            "    <td class=\"tg-j2zy\">\zxc\zxc</td>"
            "    <td class=\"tg-j2zy\">\zxc\zxc\zxc</td>"
            "  </tr>"
            "</table>"


    "</body></html>";
*/
}

void ExpenseModel::update()
{
    beginResetModel();
    d->expenses = Expense::doFetchAll(d->account);
    endResetModel();
}
