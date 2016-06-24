#include "Expense.h"

#include <QDebug>
#include <QTimeZone>

#include "Exception.h"
#include "Helpers.h"
#include "HttpRequest.h"

class ExpensePrivate
{

public:

    int id;
    int owner;
    int amount;
    bool approved;
    bool deleted;
    QDateTime epoch;
    QDateTime touched;
    QString description;
    QString comment;

    //Used for informative information in model/views.
    QString decoration ;
    ExpensePrivate()
    {
    }



    static QString dateTimeToStorableFormat(const QDateTime &dt)
    {
        return dt.toString("yyyy-MM-ddTHH:mm:ssZ");
    }

    static QDateTime storableFormatToDateTime(const QString &s)
    {
        QDateTime dt = QDateTime::fromString(s, "yyyy-MM-ddTHH:mm:ssZ");
        dt.setTimeZone(QTimeZone("UTC+00:00"));
        return dt;
    }

};

Expense::Expense(int id)
{
    d = new ExpensePrivate();
    d->id = id;
}

Expense::~Expense()
{
    delete d;
}


//
// Setters and Getters
//


int Expense::id() const
{
    return d->id;
}

int Expense::owner() const
{
    return d->owner;
}

int Expense::amount() const
{
    return d->amount;
}

bool Expense::approved() const
{
    return d->approved;
}

bool Expense::deleted()
{
    return d->deleted;
}

QDateTime Expense::epoch() const
{
    return d->epoch;
}

QDateTime Expense::touched() const
{
    return d->touched;
}

QString Expense::description() const
{
    return d->description;
}

QString Expense::comment() const
{
    return d->comment;
}

QString Expense::decoration() const
{
    return d->decoration;
}

void Expense::setOwner(int owner)
{
    d->owner = owner;
}

void Expense::setAmount(int amount)
{
    d->amount = amount;
}

void Expense::setEpoch(const QDateTime &epoch)
{
    d->epoch = epoch;
}

void Expense::setTouched(const QDateTime &touched)
{
    d->touched = touched;
}

void Expense::setDescription(const QString &description)
{
    d->description = description;
}

void Expense::setComment(const QString &comment)
{
    d->comment = comment;
}

void Expense::setDecoration(const QString &decoration)
{
    d->decoration = decoration;
}

//
// Methods to CRUD Expenses on server.
//


void Expense::doSave(pAccount account)
{
    HttpRequest req(account);
    QVariantMap data;
    data["id"] = this->id();
    data["amount"] = this->amount();
    data["description"] = this->description();
    data["comment"] = this->comment();

    try
    {
        req.doPost(QString("/expenses/%1").arg(this->id()), data);
    }
    catch(int errorcode)
    {
        QString title = QObject::tr("Could not Save Expense");
        QString noremedy = QObject::tr("There is nothing you can do about this as it is an error within the system. You should contact a system administrator.");
        switch(errorcode)
        {
        case 401: Throw(title,
                        QObject::tr("You are not authorized to update the content of this expense."),
                        QObject::tr("If you want the content of this expence changed, you must contact the user who created this expense."));
        case 404: Throw(title,
                        QObject::tr("The expense you tried to update does not exist."),
                        noremedy);
        case 409: Throw(title,
                        QObject::tr("You are trying to update an expense already approved, which is not allowed."),
                        QObject::tr("If you want to update this expense, you will have to ask your manager to unapprove it first."));
        case 410: Throw(title,
                        QObject::tr("The expense you tried to update has been deleted."),
                        noremedy);
        }
    }
}

void Expense::doDelete(pAccount account)
{
    HttpRequest req(account);
    QVariantMap data;
    data["id"] = this->id();

    try
    {
        req.doDelete(QString("/expenses/%1").arg(this->id()));
    }
    catch(int errorcode)
    {
        QString title = QObject::tr("Could not Delete Expense");
        QString noremedy = QObject::tr("There is nothing you can do about this as it is an error within the system. You should contact a system administrator.");
        switch(errorcode)
        {
        case 401: Throw(title,
                        QObject::tr("You are not authorized to delete  this expense."),
                        QObject::tr("If you want the content of this expence changed, you must contact the user who created this expense."));
        case 404: Throw(title,
                        QObject::tr("The expense you tried to delete does not exist."),
                        noremedy);
        }
    }
    this->d->deleted = true;
}

int Expense::doCreate(pAccount account, int amount, const QDateTime &epoch, const QString &description, const QString &comment)
{

    QDateTime H( QDate(1815,6,18), QTime(11,30,00));
    HttpRequest req(account);
    QVariantMap data;
    data["amount"] = amount;
    data["epoch"] = ExpensePrivate::dateTimeToStorableFormat(epoch);
    data["description"] = description;
    data["comment"] = comment;

    QVariantMap reply = req.doPut("/expenses", data);
    JsonHelpers::ensureFieldsInMap(reply, AttributeList() << Attribute("id","int"));
    return reply["id"].toInt();
}

pExpense Expense::doLoad(int id, pAccount account)
{
    HttpRequest req(account);
    QVariantMap reply = req.doGet("/expenses/"+QString::number(id))["expense"].toMap();
    JsonHelpers::ensureFieldsInMap(reply, AttributeList()
                                   << Attribute("id","int")
                                   << Attribute("amount", "int")
                                   << Attribute("owner","int")
                                   << Attribute("deleted", "int")
                                   << Attribute("approved", "int")
                                   << Attribute("description", "string")
                                   << Attribute("comment", "string")
                                   << Attribute("epoch", "string")
                                   << Attribute("touched", "string")
                                   );
    pExpense e(new Expense(reply["id"].toInt()));
    e->setOwner(reply["owner"].toInt());
    e->setAmount(reply["amount"].toInt());
    e->d->deleted  = (reply["deleted"].toInt() == 1);
    e->d->approved = (reply["approved"].toInt() == 1);

    e->setComment(reply["comment"].toString());
    e->setDescription(reply["description"].toString());

    e->setEpoch(ExpensePrivate::storableFormatToDateTime(reply["epoch"].toString()));
    e->setTouched(ExpensePrivate::storableFormatToDateTime(reply["touched"].toString()));

    return e;
}


void Expense::doApprove(pAccount account)
{
    HttpRequest req(account);
    QVariantMap data;
    data["id"] = this->id();
    data["approved"] = 1;

    try
    {
        req.doPost(QString("/expenses/%1").arg(this->id()), data);
        d->approved = true;
    }
    catch(int errorcode)
    {
        QString title = QObject::tr("Could not Save Expense");
        QString noremedy = QObject::tr("There is nothing you can do about this as it is an error within the system. You should contact a system administrator.");
        switch(errorcode)
        {
        case 401: Throw(title,
                        QObject::tr("You are not authorized to update the content of this expense."),
                        QObject::tr("If you want the content of this expence changed, you must contact the user who created this expense."));
        case 404: Throw(title,
                        QObject::tr("The expense you tried to update does not exist."),
                        noremedy);
        case 409: Throw(title,
                        QObject::tr("You are trying to update an expense already approved, which is not allowed."),
                        QObject::tr("If you want to update this expense, you will have to ask your manager to unapprove it first."));
        case 410: Throw(title,
                        QObject::tr("The expense you tried to update has been deleted."),
                        noremedy);
        }
    }
}


void Expense::doUnApprove(pAccount account)
{
    HttpRequest req(account);
    QVariantMap data;
    data["id"] = this->id();
    data["approved"] = 0;

    try
    {
        req.doPost(QString("/expenses/%1").arg(this->id()), data);
        d->approved = false;
    }
    catch(int errorcode)
    {
        QString title = QObject::tr("Could not Save Expense");
        QString noremedy = QObject::tr("There is nothing you can do about this as it is an error within the system. You should contact a system administrator.");
        switch(errorcode)
        {
        case 401: Throw(title,
                        QObject::tr("You are not authorized to update the content of this expense."),
                        QObject::tr("If you want the content of this expence changed, you must contact the user who created this expense."));
        case 404: Throw(title,
                        QObject::tr("The expense you tried to update does not exist."),
                        noremedy);
        case 409: Throw(title,
                        QObject::tr("You are trying to update an expense already approved, which is not allowed."),
                        QObject::tr("If you want to update this expense, you will have to ask your manager to unapprove it first."));
        case 410: Throw(title,
                        QObject::tr("The expense you tried to update has been deleted."),
                        noremedy);
        }
    }
}

QList<pExpense> Expense::doFetchAll(pAccount account)
{
    QList<pExpense> expenses;
    HttpRequest req(account);
    QVariantList reply = req.doGet("/expenses")["expenses"].toList();
    foreach(QVariant current, reply)
    {
        QVariantMap map = current.toMap();
        JsonHelpers::ensureFieldsInMap(map, AttributeList() <<
                                       Attribute( "id" ,  "int" ) <<
                                       Attribute("amount", "int") <<
                                       Attribute( "approved" ,  "int") <<
                                       Attribute( "deleted" ,  "int") <<
                                       Attribute( "comment" ,  "string") <<
                                       Attribute( "description" ,  "string" ) <<
                                       Attribute( "epoch" ,  "string") <<
                                       Attribute( "touched" ,  "string") <<
                                       Attribute( "owner" ,  "int") <<
                                       Attribute( "name" ,  "string") <<
                                       Attribute( "email" ,  "string")
                                       );


        Expense *e = new Expense(map["id"].toInt());
        e->setAmount(map["amount"].toInt());
        e->d->approved = (map["approved"].toInt() == 1);
        e->d->deleted = (map["deleted"].toInt() == 1);
        e->setComment(map["comment"].toString());
        e->setDescription(map["description"].toString());
        e->setEpoch(e->d->storableFormatToDateTime(map["epoch"].toString()));
        e->setTouched(e->d->storableFormatToDateTime(map["touched"].toString()));
        e->setOwner(map["owner"].toInt());
        e->setDecoration(map["name"].toString()+" <"+map["email"].toString()+">");
        expenses.append(pExpense(e));

    }

    return expenses;
}
