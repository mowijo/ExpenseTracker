#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <memory>

#include <QString>


class Account
{

public:

    Account();
    ~Account();

    QString host() const;
    int port() const;
    QString username() const;
    QString password() const;

    void setHost(const QString &host);
    void setPort(int port);
    void setUserName(const QString &username);
    void setPassword(const QString &password);

private:
    class AccountPrivate *d;
    Account(const Account &other);
    Account(const Account &&other);
    Account& operator=(Account &rhs);
};

typedef std::shared_ptr<Account> pAccount;

#endif // ACCOUNT_H

