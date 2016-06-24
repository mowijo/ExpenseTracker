#ifndef EXPENSEVIEW_H
#define EXPENSEVIEW_H

#include <QTableView>

class ExpenseView : public QTableView
{

    Q_OBJECT

public:
    ExpenseView(QWidget * parent = 0);
    ~ExpenseView();
    bool isEditing() const;
    void setModel(QAbstractItemModel *model);
    void keyPressEvent(QKeyEvent * event);
    void setFilters(const QDate &from, const QDate &to, QString text, bool show_approved, bool show_pending);

public slots:
    void approveSelection();
    void deleteSelection();

signals:
    void numberOfSelectedExpensesChanged(int unmber_of_selected_expenses);

private:
    class ExpenseViewPrivate *d;
};

#endif // EXPENSEVIEW_H
