#ifndef REPORTBUILDER_H
#define REPORTBUILDER_H


#include "Expense.h"

class ReportBuilder
{

public:

    ReportBuilder();
    ~ReportBuilder();

    void addExpense(pExpense expense);

    QString html() const;

private:
    ReportBuilder(const ReportBuilder &other);
    ReportBuilder& operator=(ReportBuilder &rhs);
    friend class ReportBuilderPrivate;
    class ReportBuilderPrivate *d;

};

#endif // REPORTBUILDER_H

