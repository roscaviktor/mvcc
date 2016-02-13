#ifndef TRANSACTIONMNG_H
#define TRANSACTIONMNG_H

#include <QObject>
#include <QList>
#include "types.h"
#include "objectmng.h"

class TransactionMng : public QObject
{
    Q_OBJECT
public:
    explicit TransactionMng(QObject *parent = 0);

    static long actionOrder();

    QList<Transaction> transactionList() const;
    void setTransactionList(const QList<Transaction> &transactionList);

    Transaction crtTransaction() const;
    void setCrtTransaction(const Transaction &crtTransaction);

    void addTransaction(const Transaction transaction);

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

    ObjectMng *m_objectMng;

    Transaction m_crtTransaction;
    QList<Transaction> m_transactionList;
};

#endif // TRANSACTIONMNG_H
