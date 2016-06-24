#include <QtTest/QtTest>
#include <QJsonDocument>

#include <Expense.h>
#include <Exception.h>

class TestExpenseAPIAccess: public QObject
{
    Q_OBJECT
private slots:


    void testCRUDExpense()
    {
        pAccount a(new Account());
        a->setPort(444);
        a->setUserName("foo@bar.com");
        a->setPassword("foobar");

        QDateTime hougoumont( QDate(1815,6,18), QTime(11,30,00), QTimeZone("UTC+00:00"));


        //Create a new expense
        int id = Expense::doCreate(a, 4223, hougoumont, "88 Pieces of artillery");

        //Load it and update it
        {
            pExpense loaded = Expense::doLoad(id, a);
            QCOMPARE(loaded->id(), id);
            QCOMPARE(loaded->amount(), 4223);
            QCOMPARE(loaded->epoch(), hougoumont);
            QCOMPARE(loaded->description(), QString("88 Pieces of artillery"));

            loaded->setAmount(123456789);
            loaded->setComment("It got a tad more expensive than anticipated");
            loaded->doSave(a);
        }

        //Load it again, verify the changes and delete it.
        {
            pExpense loaded = Expense::doLoad(id, a);

            QCOMPARE(loaded->id(), id);
            QCOMPARE(loaded->amount(), 123456789);
            QCOMPARE(loaded->epoch(), hougoumont);
            QCOMPARE(loaded->description(), QString("88 Pieces of artillery"));
            QCOMPARE(loaded->comment(), QString("It got a tad more expensive than anticipated"));

            loaded->doDelete(a);
        }

        {
            pExpense loaded = Expense::doLoad(id, a);

            QCOMPARE(loaded->id(), id);
            QCOMPARE(loaded->amount(), 123456789);
            QCOMPARE(loaded->epoch(), hougoumont);
            QCOMPARE(loaded->description(), QString("88 Pieces of artillery"));
            QCOMPARE(loaded->comment(), QString("It got a tad more expensive than anticipated"));
            QCOMPARE(loaded->deleted(), true);
        }

    }



    void testCreateApproveThenFailToUpdateUnapproveAndThenModify()
    {
        pAccount a(new Account());
        a->setPort(444);
        a->setUserName("foo@bar.com");
        a->setPassword("foobar");

        QDateTime hougoumont( QDate(1815,6,18), QTime(11,30,00), QTimeZone("UTC+00:00"));


        //Create a new expense
        int id = Expense::doCreate(a, 4223, hougoumont, "88 Pieces of artillery");


        //Lets have an approver approve it
        {
            pAccount a(new Account());
            a->setPort(444);
            a->setUserName("exp@approv.er");
            a->setPassword("Boing! Boing!");
            pExpense loaded = Expense::doLoad(id, a);
            loaded->doApprove(a);
        }

        //Load it and try to update it
        {
            pExpense loaded = Expense::doLoad(id, a);
            loaded->setAmount(123456789);
            loaded->setComment("It got a tad more expensive than anticipated");
            QVERIFY_EXCEPTION_THROWN(
                        loaded->doSave(a),
                        Exception*);
            ;
        }

        //Lets have an approver unapprove it
        {
            pAccount a(new Account());
            a->setPort(444);
            a->setUserName("exp@approv.er");
            a->setPassword("Boing! Boing!");
            pExpense loaded = Expense::doLoad(id, a);
            loaded->doUnApprove(a);
        }

        //Finally, update it.
        {
            pExpense loaded = Expense::doLoad(id, a);
            loaded->setAmount(123456789);
            loaded->setComment("It got a tad more expensive than anticipated");
            loaded->doSave(a);

            pExpense modified = Expense::doLoad(id, a);
            QCOMPARE(123456789, modified->amount());
        }

    }

    


};




int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TestExpenseAPIAccess tc;
    return QTest::qExec(&tc, argc, argv);
}


#include "TestExpenseAPIAccess.moc"

