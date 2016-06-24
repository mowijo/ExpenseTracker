#include "Account.h"

class AccountPrivate
{
public:

    QString host;
    int port;
    QString username;
    QString password;

    AccountPrivate()
    {
        host = "127.0.0.1";
        port = 443;
    }

};

Account::Account()
{
    d = new AccountPrivate;
}


Account::~Account()
{
    delete d;
}

QString Account::host() const
{
    return d->host;
}

int Account::port() const
{
    return d->port;
}

QString Account::username() const
{
    return d->username;
}

QString Account::password() const
{
    return d->password;
}

void Account::setHost(const QString &host)
{
    d->host = host;
}

void Account::setPort(int port)
{
    d->port = port;
}

void Account::setUserName(const QString &username)
{
    d->username = username;
}

void Account::setPassword(const QString &password)
{
    d->password = password;
}
