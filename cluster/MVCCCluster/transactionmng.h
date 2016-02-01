#ifndef TRANSACTIONMNG_H
#define TRANSACTIONMNG_H

#include <QObject>

class TransactionMng : public QObject
{
    Q_OBJECT
public:
    explicit TransactionMng(QObject *parent = 0);

signals:

public slots:
};

#endif // TRANSACTIONMNG_H