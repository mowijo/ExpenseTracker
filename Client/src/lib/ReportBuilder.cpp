#include "ReportBuilder.h"

#include <QObject>

#include <QMap>
#include <QList>


class Week
{
private:
    QList<pExpense> expenses;
    int week;

public:
    Week(int week)
        : week(week)
    {
    }

    void addExpense(pExpense e)
    {
        expenses.append(e);
    }


    QString html() const
    {
        int sum = 0;
        foreach(pExpense expense, expenses)
        {
            sum += expense->amount();
        }

        QString sum_as_string = QString::number(sum*0.01, 'f', 2);
        QString average_as_string = QString::number(sum*0.01/7, 'f', 2);


        QString s = "<tr>";
        s += "<td class=\"tg-j2zy\">"+QObject::tr("Week %1").arg(week)+"</td>";
        s += "<td  align=\"right\" class=\"tg-j2zy\">"+sum_as_string+"</td>";
        s += "<td align=\"right\" class=\"tg-j2zy\" >"+average_as_string+"</td>";
        s += "</tr>";
        return s;
    }
};

class Year
{
private:
    QMap<int, Week*> weeks;
    int year;

public:

    Year(int year)
        : year(year)
    {
    }

    ~Year()
    {
        foreach(int w, weeks.keys())
        {
            delete weeks.take(w);
        }
    }

    void addExpense(pExpense e, int week)
    {
        if( ! weeks.contains(week))
        {
            weeks[week] = new Week(week);
        }
        weeks[week]->addExpense(e);
    }

    QString html() const
    {
        QString s = "";

        s += "<tr><th class=\"tg-yw4l\"colspan=\"3\">" + QString::number(this->year)+"</th></tr>";
        s += "<tr><th class=\"tg-yw4l\">Week</th><th class=\"tg-yw4l\">Total (SDR) </th><th class=\"tg-yw4l\">Averrage Per Day (SDR)</th></tr>";
        QList<int> weeknumbers = weeks.keys();
        qSort(weeknumbers);
        foreach(int current_week_number, weeknumbers )
        {
            Week *week = weeks[current_week_number];
            s += week->html();
        }
        return s;

    }
};

class ReportBuilderPrivate
{

public:

    QMap<int, Year*> years;

    ReportBuilderPrivate()
    {
    }

    QList<Year*> yearsAssortedList() const
    {
        QList<int> y = years.keys();
        qSort(y);
        QList<Year*> result;
        foreach(int year, y)
        {
            result << years[year];
        }
        return result;
    }

};

ReportBuilder::ReportBuilder()
{
    d = new ReportBuilderPrivate();
}


ReportBuilder::~ReportBuilder()
{
    foreach(int year, d->years.keys())
    {
        delete d->years.take(year);
    }
    delete d;
}

void ReportBuilder::addExpense(pExpense expense)
{
    int year;
    int week;
    week = expense->epoch().toLocalTime().date().weekNumber(&year);
    if( ! d->years.contains(year) )
    {
        d->years[year] = new Year(year);
    }
    d->years[year]->addExpense(expense, week);
}

QString ReportBuilder::html() const
{
    QString s = "<html>";
    s +=
            "<head>"
            "<style type=\"text/css\">"
            ".tg  {border-collapse:collapse;border-spacing:0;border-color:#aaa;}"
            ".tg td{font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:0px;overflow:hidden;word-break:normal;border-color:#aaa;color:#333;background-color:#fff;border-top-width:1px;border-bottom-width:1px;}"
            ".tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:0px;overflow:hidden;word-break:normal;border-color:#aaa;color:#fff;background-color:#f38630;border-top-width:1px;border-bottom-width:1px;}"
            ".tg .tg-j2zy{background-color:#FCFBE3;vertical-align:top;}"
            ".tg .tg-yw4l{vertical-align:top}"

            "</style>"
            "</head>"
            "<body>";

    s+= "<h1>"+QObject::tr("Expenses")+"</h1>";
    s+= "<table width=\"100%\" class=\"tg\">";
    QList<Year*> years = d->yearsAssortedList();
    foreach(Year *year, years)
    {
        s += year->html();
    }

    s += "</table></body></html>";
    return s;

}
