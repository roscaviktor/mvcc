#ifndef OBJECTMNG_H
#define OBJECTMNG_H

#include <QObject>
#include "global.h"
#include "types.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class ObjectMng : public QObject
{
    Q_OBJECT
public:
    explicit ObjectMng(QObject *parent = 0);

    QList<TrObject> objectList() const;
    void setObjectList(const QList<TrObject> &objectList);

    QString objectListStr() const;
    void setObjectListStr(const QString &objectListStr);

    void addObject(TrObject obj);
    void exec(TrAction *action, long ts);

signals:
    bool sync(QString);

public slots:
    void init();

private:
    QList<TrObject> m_objectList;   // object struct list
    QString m_objectListStr;        // object struct list to string

    QSqlDatabase db;
};

#endif // OBJECTMNG_H
