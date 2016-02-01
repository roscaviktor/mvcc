#ifndef TYPES_H
#define TYPES_H
#include <QObject>
#include <QVariant>

typedef struct _TrObject{
    long version;
    long rts;
    long wts;
    QString value;
    QString name;
}TrObject;

typedef enum _TrActionType{
    READ,
    WRITE
}TrActionType;

typedef struct _TrAction{
    TrActionType type;
    TrObject *trObject;
}TrAction;

typedef enum _TrStatus{
    STARTED,
    COMMITED,
    ABORTED
}TrStatus;

typedef struct _Transaction{
    long ts;
    QString name;
    TrStatus status;
    QList<TrAction> actionList;
}Transaction;

#endif // TYPES_H
