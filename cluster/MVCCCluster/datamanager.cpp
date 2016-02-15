#include "datamanager.h"
#include <QDebug>
#include <QDateTime>
#include <QMutexLocker>


////////////////////////////////////////////////////////////
DataManager::DataManager(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DB_NAME);
    if(db.open()){
        DEBUG_SQLITE(QString("Open database %1!").arg(DB_NAME))
    }else{
        DEBUG_SQLITE(QString("Error to open database %1!").arg(DB_NAME))
    }
    m_objectList.clear();
}

////////////////////////////////////////////////////////////
void DataManager::add(TrObject obj)
{
    m_objectList.append(obj);
}

////////////////////////////////////////////////////////////
void DataManager::init()
{
    TrObject obj;
    QSqlQuery query(db);
    QString str;

    QStringList tables = db.tables();

    foreach (obj, m_objectList) {
        if(tables.contains(obj.name)){
            DEBUG_SQLITE(QString("Table %1 already exist.").arg(obj.name))
            continue;
        }
        query.prepare(QString("CREATE TABLE IF NOT EXISTS %1 "
                      "(version INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, "
                      "value VARCHAR(200),"
                      "rts DATETIME DEFAULT CURRENT_TIMESTAMP,"
                      "wts DATETIME DEFAULT CURRENT_TIMESTAMP)").arg(obj.name));
        if(query.exec()){
            DEBUG_SQLITE(QString("Created table %1").arg(obj.name));
            str = QString("INSERT INTO %1 (rts, wts, value) VALUES ('%2', '%3', '%4');")
                    .arg(obj.name)
                    .arg(obj.rts)
                    .arg(obj.wts)
                    .arg(obj.value);
            DEBUG_SQLITE(str)
            if(query.exec(str) == false){
                DEBUG_SQLITE("Error: " + query.lastError())
            }
        }else{
            DEBUG_SQLITE("Error: " + query.lastError())
        }
    }
}

bool DataManager::exec(TrAction tr)
{
    QMutexLocker ml(&mMutex);

    if(tr.type == READ){
        return read(tr);
    }
    return write(tr);
}


////////////////////////////////////////////////////////////
bool DataManager::write(TrAction action)
{
    QSqlQuery query(db);
    QString str, jsonStr;
    TrObject obj;

    // read rts
    str = QString("SELECT * FROM %1 ORDER BY rts DESC LIMIT 1;").arg(action.trObject.name);
    DEBUG_SQLITE(str)
    if(query.exec(str) && query.next()){
//          obj.version = query.value("version").toLongLong();
            obj.rts = query.value("rts").toLongLong();
//          obj.wts = query.value("wts").toLongLong();
//          obj.value = query.value("value").toString();
     }else{
        DEBUG_SQLITE("Error: " + query.lastError())
        return false;
     }

    if(action.ts < obj.rts){
        // abort!
        DEBUG_ACTION(QString("=========== %1 WRITE ============").arg(action.trName))
        DEBUG_ACTION("Abort!!!")
        return false;
    }
    // write
    action.trObject.wts = action.ts;
    action.trObject.rts = obj.rts ;
    str = QString("INSERT INTO %1 (rts, wts, value) VALUES ('%2', '%3', '%4');")
            .arg(action.trObject.name)
            .arg(action.trObject.rts)
            .arg(action.trObject.wts)
            .arg(action.trObject.value);
    DEBUG_SQLITE(str)
    if(query.exec(str)){
        DEBUG_ACTION(QString("=========== %1 WRITE ============").arg(action.trName))
        DEBUG_ACTION(QString("[%1]%2: %3").arg(action.trName).arg("name:").arg(action.trObject.name))
        DEBUG_ACTION(QString("[%1]%2: %3").arg(action.trName).arg("ts:").arg(action.ts))
        DEBUG_ACTION(QString("[%1]%2: %3").arg(action.trName).arg("rts:").arg(action.trObject.rts))
        DEBUG_ACTION(QString("[%1]%2: %3").arg(action.trName).arg("wts:").arg(action.trObject.wts))
        DEBUG_ACTION(QString("[%1]%2: %3").arg(action.trName).arg("value:").arg(action.trObject.value))
        emit sync(jsonStr);
    }else{
        DEBUG_ACTION(QString("=========== %1 WRITE ============").arg(action.trName))
        DEBUG_SQLITE("Error: " + query.lastError())
    }

    return true;
}

////////////////////////////////////////////////////////////
bool DataManager::read(TrAction action)
{
    QSqlQuery query(db);
    QString str;
    TrObject obj;

    str = QString("SELECT * FROM %1 WHERE wts < %2 LIMIT 1;").arg(action.trObject.name).arg(action.ts);
    DEBUG_SQLITE(str)
    if(query.exec(str) && query.next()){
//        obj.version = query.value("version").toLongLong();
        obj.rts = query.value("rts").toLongLong();
        obj.wts = query.value("wts").toLongLong();
        obj.value = query.value("value").toString();
    }else{
        DEBUG_SQLITE("Error: " + query.lastError())
        return false;
    }

    action.trObject.rts = obj.rts;
    action.trObject.wts = obj.wts;
    action.trObject.value = obj.value;

    if(action.trObject.rts < action.ts){
        action.trObject.rts = action.ts;

        str = QString("INSERT INTO %1 (rts, wts, value) VALUES ('%2', '%3', '%4');")
                .arg(action.trObject.name)
                .arg(action.trObject.rts)
                .arg(action.trObject.wts)
                .arg(action.trObject.value);
        DEBUG_SQLITE(str)
        if(query.exec(str)){
//            emit sync(jsonStr);
            DEBUG_ACTION(QString("=========== %1 READ ============").arg(action.trName))
            DEBUG_ACTION(QString("[%1]%2: %3").arg(action.trName).arg("name:").arg(action.trObject.name))
            DEBUG_ACTION(QString("[%1]%2: %3").arg(action.trName).arg("ts:").arg(action.ts))
            DEBUG_ACTION(QString("[%1]%2: %3").arg(action.trName).arg("rts:").arg(action.trObject.rts))
            DEBUG_ACTION(QString("[%1]%2: %3").arg(action.trName).arg("wts:").arg(action.trObject.wts))
            DEBUG_ACTION(QString("[%1]%2: %3").arg(action.trName).arg("value:").arg(action.trObject.value))
        }else{
            DEBUG_ACTION(QString("=========== %1 READ ============").arg(action.trName))
            DEBUG_SQLITE("Error: " + query.lastError())
            return false;
        }
    }

    return true;
}


////////////////////////////////////////////////////////////
void DataManager::approve(QString jsonStr){
    TrObject obj;
    QSqlQuery query(db);
    QString str = QString("INSERT INTO %1 (rts, wts, value) VALUES (%2, %3, '%4');")
            .arg(obj.name).arg(obj.rts).arg(obj.wts).arg(obj.value);

    if(query.exec()){
//        emit updated("ACK");
    }else{
        qDebug() << query.lastError();
    }
    qDebug() << str;
}

QList<TrObject> DataManager::objectList() const
{
    return m_objectList;
}

void DataManager::setObjectList(const QList<TrObject> &objectList)
{
    m_objectList = objectList;
}

QString DataManager::objectListStr() const
{
    return m_objectListStr;
}

void DataManager::setObjectListStr(const QString &objectListStr)
{
    m_objectListStr = objectListStr;
}

QStringList DataManager::objectQStringList()
{
    return db.tables();
}
