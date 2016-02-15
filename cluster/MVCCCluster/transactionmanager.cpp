#include "transactionmanager.h"
#include <QThread>
#include <QDateTime>
#include <QDebug>

long TransactionManager::m_actionOrder = 0;

TransactionManager::TransactionManager(QObject *parent) : QObject(parent)
{
    m_objectMng = new DataManager();

    connect(this, SIGNAL(next()), this, SLOT(nextAction()));
    connect(this, SIGNAL(commited()), this, SLOT(commit()));
    connect(this, SIGNAL(aborted()), this, SLOT(abort()));
}

long TransactionManager::actionOrder()
{
    return m_actionOrder++;
}

void TransactionManager::run()
{
    if(m_transactionList.size()){
        m_crtTransaction = m_transactionList.first();
//        qDebug() << QString("[%1] %2 Start").arg(this->objectName()).arg(m_crtTransaction.name);
        nextAction();
    }else{
        qDebug() << "END";
    }
}

void TransactionManager::nextAction()
{
    if(m_crtTransaction.actionList.size()){
        TrAction action;
        action = m_crtTransaction.actionList.first();
        m_crtTransaction.actionList.pop_front();
        action.ts = m_crtTransaction.ts;
        action.trName = m_crtTransaction.name;
        QThread::msleep(rand() % 10);
        QString table, tmp;
        tmp = "";
        if(m_objectMng->exec(action)){
            if(action.type == READ ){
                foreach (table, m_tables) {
                    if(table.size() > 1)
                        continue;
                    //transaction, operation, status
                    if(table == action.trObject.name){
                        // | T1 W C |
                        tmp += QString("| %1 %2   ").arg(m_crtTransaction.name).arg(action.type == READ ? "R" : "W");
                    }else{
                        tmp += "| -------- ";
                    }
                }
                // timestamp, thread, objectList
                qDebug() << QString("[%1][%2] %3 ").arg(m_crtTransaction.ts).arg(this->objectName()).arg(tmp);
                emit next();
            }
            else{
                foreach (table, m_tables) {
                    if(table.size() > 1)
                        continue;
                    //transaction, operation, status
                    if(table == action.trObject.name){
                        // | T1 W C |
                        if(m_crtTransaction.actionList.size())
                            tmp += QString("| %1 W   ").arg(m_crtTransaction.name);
                        else
                            tmp += QString("| %1 W C.").arg(m_crtTransaction.name);
                    }else{
                        tmp += "| -------- ";
                    }
                }
                // timestamp, thread, objectList
                qDebug() << QString("[%1][%2] %3 ").arg(m_crtTransaction.ts).arg(this->objectName()).arg(tmp);
                if(m_crtTransaction.actionList.size())
                    emit next();
                else
                    emit commited();
            }
        }
        else{
            foreach (table, m_tables) {
                if(table.size() > 1)
                    continue;
                //transaction, operation, status
                if(table == action.trObject.name){
                    // | T1 W C |
                    tmp += QString("| %1 %2 A!").arg(m_crtTransaction.name).arg(action.type == READ ? "R" : "W");
                }else{
                    tmp += "| -------- ";
                }
            }
            // timestamp, thread, objectList
            qDebug() << QString("[%1][%2] %3 ").arg(m_crtTransaction.ts).arg(this->objectName()).arg(tmp);
            //qDebug() << QString("[%1 %5] %2 %3 %4 Abort!!!").arg(this->objectName()).arg(m_crtTransaction.name)
                        //.arg(action.type == READ ? "Read" : "Write").arg(action.trObject.name).arg(m_crtTransaction.ts);
            emit aborted();
        }
    }   
}

void TransactionManager::commit()
{
    m_transactionList.pop_front();
    m_crtTransaction.status == COMMITED;
    run();
}

void TransactionManager::abort()
{
    m_crtTransaction = m_transactionList.first();
    m_crtTransaction.ts = QDateTime::currentMSecsSinceEpoch();
    QThread::msleep(1);
    m_transactionList.push_back(m_crtTransaction);
    m_transactionList.pop_front();
    m_crtTransaction.status == ABORTED; //oreder is importand !
    run();
}

QStringList TransactionManager::tables() const
{
    return m_tables;
}

void TransactionManager::setTables(const QStringList &tables)
{
    m_tables = tables;
}

Transaction TransactionManager::crtTransaction() const
{
    return m_crtTransaction;
}

void TransactionManager::setCrtTransaction(const Transaction &crtTransaction)
{
    m_crtTransaction = crtTransaction;
}

void TransactionManager::addTransaction(const Transaction transaction)
{
    m_transactionList.push_back(transaction);
}

QList<Transaction> TransactionManager::transactionList() const
{
    return m_transactionList;
}

void TransactionManager::setTransactionList(const QList<Transaction> &transactionList)
{
    m_transactionList = transactionList;
}
