#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDateTime>
#include "datamanager.h"
#include "transactionmanager.h"
#include <QObject>
#include <QThread>
#include <QDebug>


void initTransactionManager(TransactionManager *transactionMng, int offset){
    // add transactions to transactionMng on Thread 1
    int i = offset * MAX_TRANSACTION_NO + 1;
    int total = i + MAX_TRANSACTION_NO;

    while(i < total){
        Transaction transaction;
        int j = 0;
        while(j < MAX_OBJECT_PER_TRANSACTION){
            TrObject object;
            TrAction action1, action2;

            object.name = QString("%1").arg(QChar('A' + char(rand() % ('H' - 'A'))));
            object.rts = 0;
            object.wts = 0;
            object.value = QString("%1").arg(i);

            char tmp[4];
            sprintf(tmp, "T%03d", i);
            transaction.name = QString("%1").arg(tmp);
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
            j++;
        }
        transactionMng->addTransaction(transaction);

        i++;
    }
}


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    DataManager *objectMng = new DataManager();

    for(int i=int('A'); i<int('H'); i++){
        TrObject object;
        object.name = QString("%1").arg(QChar(i));
        object.rts = 0;
        object.wts = 0;
        object.value = "1";
        objectMng->add(object);
    }

    objectMng->init();

    QStringList tables = objectMng->objectQStringList();
    QString objectList, tmp;
    foreach (objectList, tables) {
        if(objectList.size() > 1)
            continue;
        tmp += QString("|     %1    ").arg(objectList);
    }

    TransactionManager *transactionMng = new TransactionManager();
    TransactionManager *transactionMng2 = new TransactionManager();

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

    initTransactionManager(transactionMng, 0);
    initTransactionManager(transactionMng2, 1);

    transactionMng->setObjectName("thread_1");
    transactionMng2->setObjectName("thread_2");

    transactionMng->setTables(tables);
    transactionMng2->setTables(tables);

    qDebug() << "";
    qDebug() << QString("------------------------- %1.").arg(tmp);

    thread2->start();
    thread->start();

//    QQmlApplicationEngine engine;
//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
