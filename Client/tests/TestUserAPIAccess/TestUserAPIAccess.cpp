#include <QtTest/QtTest>
#include <QJsonDocument>

#include <User.h>
#include <Exception.h>

class TestUserAPIAccess: public QObject
{
    Q_OBJECT
private slots:

    void testLoadAndSave()
    {
        pAccount a(new Account());
        a->setPort(444);
        a->setUserName("foo@bar.com");
        a->setPassword("foobar");

        pUser u = User::doLoad("foo@bar.com", a);
        QCOMPARE( u->name(), QString("Fubber (pwd 'foobar')"));

        u->setName("Nicholas Tesla");
        u->doSave(a);
    }


    void testBadCredentials()
    {
        pAccount a(new Account());
        a->setPort(444);
        a->setUserName("foo@bar.com");
        a->setPassword("No worky worky");

        QVERIFY_EXCEPTION_THROWN(
                    User::doLoad("foo@bar.com", a),
                    Exception*);

    }


    void testCreateNewUser()
    {
        pAccount a(new Account());
        a->setPort(444);
        QString password = "no worky worky";
        QString newemail = "bongo@haveloc.sl";
        QString newname = "Bongo the Hippo";

        a->setUserName(newemail);
        a->setPassword(password);

        //CAnnot load Bongo
        QVERIFY_EXCEPTION_THROWN(
                    User::doLoad(newemail, a),
                    Exception*);

        //Creates "bongo"
        password = User::doCreate(newemail, newname, a);

        //Now, bongo can login wth his new password.
        a->setPassword(password);
        User::doLoad(newemail, a);

        //We cannot create a new user with same email address....
        QVERIFY_EXCEPTION_THROWN(
                    User::doCreate(newemail, newname, a),
                    Exception*);
    }

    void testFetchAllUsers()
    {
        pAccount a(new Account());
        a->setPort(444);
        a->setUserName("root");
        a->setPassword("foobar");

        QList<pUser> users = User::doFetchAll(a);
        QCOMPARE(2, users.length());
        foreach(pUser user, users)
        {
            switch(user->id())
            {
            case 1:
                QCOMPARE(user->name(), QString("Super duper user (pwd 'foobar')"));
                QCOMPARE(user->email(), QString("root"));
                QCOMPARE(user->privileges(), 0);
                break;
            case 2:
                QCOMPARE(user->name(), QString("Fubber (pwd 'foobar')"));
                QCOMPARE(user->email(), QString("foo@bar.com"));
                QCOMPARE(user->privileges(), 0);
                break;
            default:
                QFAIL("Unexpected id");
            }
        }
    }


    void testFetchAllUsersWithUnauthorizedUser()
    {
        pAccount a(new Account());
        a->setPort(444);
        a->setUserName("foo@bar.com");
        a->setPassword("foobar");
        QList<pUser> users;
        QVERIFY_EXCEPTION_THROWN(
                    users = User::doFetchAll(a),
                    Exception*);
        QCOMPARE(0, users.length());
    }




};




int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TestUserAPIAccess tc;
    return QTest::qExec(&tc, argc, argv);
}


#include "TestUserAPIAccess.moc"

