#include "User.h"

#include <QDebug>

#include "Helpers.h"
#include "HttpRequest.h"
#include "Exception.h"

class UserPrivate
{
public:

    static const int MAY_APPROVE_EXPENSES;
    static const int MAY_MANAGE_USERS;
    int id;
    QString email;
    QString name;
    int privileges;

    UserPrivate() :
        id(0),
        email(""),
        name(""),
        privileges(0)
    {
    }
};

const int UserPrivate::MAY_APPROVE_EXPENSES = 0x01;
const int UserPrivate::MAY_MANAGE_USERS     = 0x02;

User::User(int id)
{
    d = new UserPrivate;
    d->id = id;
}

User::User()
{
    d = new UserPrivate;
}

User::~User()
{
    delete d;
}

int User::id() const
{
    return d->id;
}

QString User::name() const
{
    return d->name;
}

QString User::email() const
{
    return d->email;
}

int User::privileges() const
{
    return d->privileges;
}

bool User::mayApproveExpenses() const
{
    return d->privileges & UserPrivate::MAY_APPROVE_EXPENSES;
}

bool User::mayManageUsers() const
{
    return d->privileges & UserPrivate::MAY_MANAGE_USERS;
}

void User::setMayApproveExpenses(bool may)
{
    if(may)
    {
        d->privileges |= UserPrivate::MAY_APPROVE_EXPENSES;
    }
    else
    {
        d->privileges &= ~UserPrivate::MAY_APPROVE_EXPENSES;
    }
}

void User::setMayManageUsers(bool may)
{
    if(may)
    {
        d->privileges |= UserPrivate::MAY_MANAGE_USERS;
    }
    else
    {
        d->privileges &= ~UserPrivate::MAY_MANAGE_USERS;
    }
}

void User::setEmail(const QString &email)
{
    d->email = email;
}

void User::setName(const QString &name)
{
    d->name = name;
}

void User::setPrivileges(int privileges)
{
    d->privileges = privileges;
}

void User::doSave(pAccount account)
{
    HttpRequest req(account);
    QVariantMap data;
    data["email"] = this->email();
    data["name"] = this->name();
    data["privileges"] = this->privileges();
    data["id"] = this->id();
    try
    {
        QVariantMap map = req.doPost(QString("/users/%1").arg(this->id()), data);
    }
    catch(int errorcode)
    {
        switch(errorcode)
        {
            case 409: Throw(QObject::tr("Could not Save User"),
                            QObject::tr("The user could not be saved because the email address <b>%1</b> is already in use by another user.").arg(this->email()),
                            QObject::tr("Select another email address or modify or remove the user aldready using %1").arg(this->email()));
        }
    }
}


QString User::doCreate(const QString &email, const QString &name, pAccount account)
{
    HttpRequest req(account);
    QVariantMap data;
    data["email"] = email;
    data["name"] = name;
    QVariantMap reply = req.doPut("/users", data);
    JsonHelpers::ensureFieldsInMap(reply, AttributeList() << Attribute("password","string"));
    return reply["password"].toString();
}

pUser User::doLoad(const QString &email, pAccount account)
{
    HttpRequest req(account);
    QVariantMap reply = req.doGet("/users/"+email)["user"].toMap();
    JsonHelpers::ensureFieldsInMap(reply, AttributeList() << Attribute("id","int") << Attribute("email", "string") << Attribute("name","string") << Attribute("privileges", "int"));
    pUser u(new User(reply["id"].toInt()));
    u->setName(reply["name"].toString());
    u->setEmail(reply["email"].toString());
    u->setPrivileges(reply["privileges"].toInt());
    return u;
}

QList<pUser> User::doFetchAll(pAccount account)
{
    QList<pUser> users;
    HttpRequest req(account);
    QVariantList reply = req.doGet("/users")["users"].toList();
    foreach(QVariant currentvariat, reply)
    {
        QVariantMap map = currentvariat.toMap();
        JsonHelpers::ensureFieldsInMap(map, AttributeList() << Attribute("id","int") << Attribute("email", "string") << Attribute("name","string") << Attribute("privileges", "int"));

        pUser u(new User(map["id"].toInt()));
        u->setName(map["name"].toString());
        u->setEmail(map["email"].toString());
        u->setPrivileges(map["privileges"].toInt());
        users.append(u);
    }
    return users;

}
