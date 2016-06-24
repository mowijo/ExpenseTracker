#ifndef USER_H
#define USER_H

#include <memory>

#include <QString>

#include "Account.h"

typedef std::shared_ptr<class User> pUser;

class User
{

public:

    User();
    ~User();

    int id() const;
    QString email() const;
    QString name() const;
    int privileges() const;


    bool mayApproveExpenses() const;
    bool mayManageUsers() const;
    void setMayApproveExpenses(bool may);
    void setMayManageUsers(bool may);

    void setEmail(const QString &email);
    void setName(const QString &name);
    void setPrivileges(int privileges);

    void doSave(pAccount account);
    static QString doCreate(const QString &email, const QString &name, pAccount account);
    static pUser doLoad(const QString &email, pAccount account);
    static QList<pUser> doFetchAll(pAccount account);



private:
    explicit User(int id);
    class UserPrivate *d;
    User &operator=(const User &rhs);
    User(const User &other);
    User(const User &&other);



};


#endif /* USER_H */

