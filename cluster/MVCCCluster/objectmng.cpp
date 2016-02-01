#include "objectmng.h"
#include <QDebug>
#include <QDateTime>

////////////////////////////////////////////////////////////
ObjectMng::ObjectMng(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DB_NAME);
    if(db.open()){
        qDebug() << QString("Open database %1!").arg(DB_NAME);
    }else{
        qDebug() << QString("Error to open database %1!").arg(DB_NAME);
    }
    m_objectList.clear();
}

QList<TrObject> ObjectMng::objectList() const
{
    return m_objectList;
}

void ObjectMng::setObjectList(const QList<TrObject> &objectList)
{
    m_objectList = objectList;
}

QString ObjectMng::objectListStr() const
{
    return m_objectListStr;
}

void ObjectMng::setObjectListStr(const QString &objectListStr)
{
    m_objectListStr = objectListStr;
}

void ObjectMng::addObject(TrObject obj)
{
    m_objectList.append(obj);
}

////////////////////////////////////////////////////////////
void ObjectMng::exec(TrAction *action, long ts)
{
    QSqlQuery query(db);
    QString str;
    TrObject obj;

    str = QString("SELECT * FROM %1 WHERE wts < %2 LIMIT 1;").arg(action->trObject->name).arg(ts);
    if(query.exec() && query.next()){
        obj.version = query.value("version").toLongLong();
        obj.rts = query.value("rts").toLongLong();
        obj.wts = query.value("wts").toLongLong();
        obj.value = query.value("value").toString();
    }else{
        qDebug() << query.lastError();
    }
    qDebug() << str;

    if(action->type == READ){
        action->trObject = obj;
        action->trObject->rts = ts;
    }else{
        if(tr >= obj.rts){
            action->trObject->wts = ts;
        }
    }

    str = QString("INSERT INTO %1 (rts, wts, value) VALUES (%2, %3, '%4');")
            .arg(action->trObject->name)
            .arg(action->trObject->rts)
            .arg(action->trObject->wts)
            .arg(action->trObject->value);

    if(query.exec()){
        emit sync(jsonStr);
    }else{
        qDebug() << query.lastError();
    }
    qDebug() << str;
}

////////////////////////////////////////////////////////////
void approve(QString jsonStr){
    TrObject obj;

    str = QString("INSERT INTO %1 (rts, wts, value) VALUES (%2, %3, '%4');")
            .arg(obj.name).arg(obj.rts).arg(obj.wts).arg(obj.value);

    if(query.exec()){
        emit updated("ACK");
    }else{
        qDebug() << query.lastError();
    }
    qDebug() << str;
}

////////////////////////////////////////////////////////////
void ObjectMng::init()
{
    TrObject obj;
    QSqlQuery query(db);

    QStringList tables = db.tables();

    foreach (obj, m_objectList) {
        if(tables.contains(obj.name)){
            qDebug() << QString("Table %1 already exist.").arg(obj.name);
            continue;
        }
        query.prepare(QString("CREATE TABLE IF NOT EXISTS %1 "
                      "(version INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, "
                      "value VARCHAR(200),"
                      "rts DATETIME DEFAULT CURRENT_TIMESTAMP,"
                      "wts DATETIME DEFAULT CURRENT_TIMESTAMP)").arg(obj.name));
        if(query.exec()){
            qDebug() << QString("Created table %1").arg(obj.name);
        }else{
            qDebug() << query.lastError();
        }
    }
}
