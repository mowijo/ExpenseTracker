#include <QtTest/QtTest>

#include <User.h>

class TestUser: public QObject
{
    Q_OBJECT
private slots:

    void testPrivileges()
    {
        User u;
        QVERIFY( ! u.mayApproveExpenses() );
        QVERIFY( ! u.mayManageUsers() );

        u.setMayApproveExpenses(true);
        QVERIFY(  u.mayApproveExpenses() );
        QVERIFY( ! u.mayManageUsers() );

        u.setMayManageUsers(true);
        QVERIFY(  u.mayApproveExpenses() );
        QVERIFY(  u.mayManageUsers() );

        u.setMayApproveExpenses(false);
        QVERIFY( ! u.mayApproveExpenses() );
        QVERIFY(   u.mayManageUsers() );

        u.setMayManageUsers(false);
        QVERIFY( ! u.mayApproveExpenses() );
        QVERIFY( ! u.mayManageUsers() );

    }


};




int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TestUser tc;
    return QTest::qExec(&tc, argc, argv);
}


#include "TestUser.moc"

