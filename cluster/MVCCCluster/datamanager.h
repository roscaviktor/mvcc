#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include "global.h"
#include "types.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QThread>
#include <QMutex>

#ifdef DEBUG_SQL
    #define DEBUG_SQLITE(str) qDebug() << << str;
#else
    #define DEBUG_SQLITE(str)
#endif

#ifdef DEBUG_ACTIONS
//    #define DEBUG_ACTION(str) qDebug() << QThread::currentThread() << str;
    #define DEBUG_ACTION(str) qDebug() << str;
#else
    #define DEBUG_ACTION(str)
#endif


class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = 0);

    void add(TrObject obj);
    bool exec(TrAction);

    QList<TrObject> objectList() const;
    void setObjectList(const QList<TrObject> &objectList);

    QString objectListStr() const;
    void setObjectListStr(const QString &objectListStr);

    QStringList objectQStringList();

signals:
    void sync(QString);
    void actionDone();

public slots:
    void init();

private:
    bool read(TrAction action);
    bool write(TrAction action);

    void approve(QString jsonStr);

    QList<TrObject> m_objectList;   // object struct list
    QString m_objectListStr;        // object struct list to string

    QSqlDatabase db;

    QMutex mMutex;
};

#endif // DATAMANAGER_H
