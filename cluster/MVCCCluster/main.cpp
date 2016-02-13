#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDateTime>
#include "objectmng.h"
#include "transactionmng.h"
#include <QObject>
#include <QThread>


void initTransactionMng(TransactionMng *transactionMng, int offset){
    TrObject objectX;
    objectX.name = "X";
    objectX.rts = 0;
    objectX.wts = 0;
    objectX.value = "1";

    TrObject objectY;
    objectY.name = "Y";
    objectY.rts = 0;
    objectY.wts = 0;
    objectY.value = "1";


    TrObject object;
    TrAction action1, action2;
    Transaction transaction;

    // add transactions to transactionMng on Thread 1
    int i = offset * MAX_TRANSACTION_NO + 1;
    int total = i + MAX_TRANSACTION_NO;
    while(i < total){

        if(rand()%2){
            object = objectX;
        }else{
            object = objectY;
        }

        transaction.name = QString("T%1").arg(i);
        transaction.ts = QDateTime::currentMSecsSinceEpoch();
        transaction.status = STARTED;
        QThread::msleep(1);

        // add actions to transaction
        action1.trObject = object;
        action1.type = READ;
        transaction.actionList.append(action1);

        object.value = QString("val%1").arg(i);
        action2.trObject = object;
        action2.type = WRITE;
        transaction.actionList.append(action2);

        transactionMng->addTransaction(transaction);

        i++;
    }
}


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    ObjectMng *objectMng = new ObjectMng();

    TrObject obj;
    obj.name = "Y";
    obj.rts = 0;
    obj.wts = 0;
    obj.value = "1";
    objectMng->add(obj);

    TrObject obj1;
    obj1.name = "Y";
    obj1.rts = 0;
    obj1.wts = 0;
    obj1.value = "1";
    objectMng->add(obj1);

    objectMng->init();

    TransactionMng *transactionMng = new TransactionMng();
    TransactionMng *transactionMng2 = new TransactionMng();

    QThread *thread = new QThread();
    QThread *thread2 = new QThread();

    thread->setObjectName("Thread_1");
    thread2->setObjectName("Thread_2");

    transactionMng->moveToThread(thread);
    transactionMng2->moveToThread(thread2);
    QObject::connect(thread, SIGNAL(started()), transactionMng, SLOT(run()));
    QObject::connect(thread2, SIGNAL(started()), transactionMng2, SLOT(run()));
//    connect( task, SIGNAL(workFinished()), thread, SLOT(quit()) );
    //automatically delete thread and task object when work is done:
//    connect( thread, SIGNAL(finished()), task, SLOT(deleteLater()) );
    QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    QObject::connect(thread2, SIGNAL(finished()), thread2, SLOT(deleteLater()));

    initTransactionMng(transactionMng, 0);
    initTransactionMng(transactionMng2, 1);

    transactionMng->setObjectName("thread_1");
    transactionMng2->setObjectName("thread_2");

    thread2->start();
    thread->start();

//    QQmlApplicationEngine engine;
//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
