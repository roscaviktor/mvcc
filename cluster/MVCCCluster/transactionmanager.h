#ifndef TRANSACTIONMANAGER_H
#define TRANSACTIONMANAGER_H

#include <QObject>
#include <QList>
#include "types.h"
#include "datamanager.h"
#include <QMutex>
#include <QMutexLocker>

class TransactionManager : public QObject
{
    Q_OBJECT
public:
    explicit TransactionManager(QObject *parent = 0);

    static long actionOrder();

    QList<Transaction> transactionList() const;
    void setTransactionList(const QList<Transaction> &transactionList);

    Transaction crtTransaction() const;
    void setCrtTransaction(const Transaction &crtTransaction);

    void addTransaction(const Transaction transaction);

    QStringList tables() const;
    void setTables(const QStringList &tables);

signals:
    void exec(TrAction);
    void next();
    void commited();
    void aborted();

public slots:
    void nextAction();
    void run();
    void commit();
    void abort();

private:
    static long m_actionOrder;

    DataManager *m_objectMng;

    Transaction m_crtTransaction;
    QList<Transaction> m_transactionList;

    QStringList m_tables;

    QMutex mMutex;
};

#endif // TRANSACTIONMANAGER_H
