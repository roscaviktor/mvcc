#include "transactionmng.h"
#include <QThread>
#include <QDateTime>
#include <QDebug>

long TransactionMng::m_actionOrder = 0;

TransactionMng::TransactionMng(QObject *parent) : QObject(parent)
{
    m_objectMng = new ObjectMng();

    connect(this, SIGNAL(next()), this, SLOT(nextAction()));
    connect(this, SIGNAL(commited()), this, SLOT(commit()));
    connect(this, SIGNAL(aborted()), this, SLOT(abort()));
}

long TransactionMng::actionOrder()
{
    return m_actionOrder++;
}

void TransactionMng::run()
{
    if(m_transactionList.size()){
        m_crtTransaction = m_transactionList.first();
//        qDebug() << QString("[%1] %2 Start").arg(this->objectName()).arg(m_crtTransaction.name);
        nextAction();
    }
}

void TransactionMng::nextAction()
{
    if(m_crtTransaction.actionList.size()){
        TrAction action;
        action = m_crtTransaction.actionList.first();
        m_crtTransaction.actionList.pop_front();
        action.ts = m_crtTransaction.ts;
        action.trName = m_crtTransaction.name;
        QThread::msleep(rand() % 100);
        if(m_objectMng->exec(action)){
            if(action.type == READ ){
                qDebug() << QString("[%1 %5] %2 %3 %4").arg(this->objectName()).arg(m_crtTransaction.name)
                            .arg("Read").arg(action.trObject.name).arg(m_crtTransaction.ts);
                emit next();
            }
            else{
                qDebug() << QString("[%1 %5] %2 Write %3 Commit").arg(this->objectName()).arg(m_crtTransaction.name)
                            .arg(action.trObject.name).arg(m_crtTransaction.ts);
                emit commited();
            }
        }
        else{
            qDebug() << QString("[%1 %5] %2 %3 %4 Abort!!!").arg(this->objectName()).arg(m_crtTransaction.name)
                        .arg(action.type == READ ? "Read" : "Write").arg(action.trObject.name).arg(m_crtTransaction.ts);
            emit aborted();
        }
    }
}

void TransactionMng::commit()
{
    m_transactionList.pop_front();
    m_crtTransaction.status == COMMITED;
    run();
}

void TransactionMng::abort()
{
    m_crtTransaction = m_transactionList.first();
    m_crtTransaction.ts = QDateTime::currentMSecsSinceEpoch();
    QThread::msleep(1);
    m_transactionList.push_back(m_crtTransaction);
    m_transactionList.pop_front();
    m_crtTransaction.status == ABORTED; //oreder is importand !
    run();
}

Transaction TransactionMng::crtTransaction() const
{
    return m_crtTransaction;
}

void TransactionMng::setCrtTransaction(const Transaction &crtTransaction)
{
    m_crtTransaction = crtTransaction;
}

void TransactionMng::addTransaction(const Transaction transaction)
{
    m_transactionList.push_back(transaction);
}

QList<Transaction> TransactionMng::transactionList() const
{
    return m_transactionList;
}

void TransactionMng::setTransactionList(const QList<Transaction> &transactionList)
{
    m_transactionList = transactionList;
}
