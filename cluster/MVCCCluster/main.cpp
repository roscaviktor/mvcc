#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "objectmng.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    ObjectMng *objectMng = new ObjectMng();

    TrObject obj;
    obj.name = "X";
    obj.rts = 0;
    obj.wts = 0;
    obj.value = "1";
    objectMng->addObject(obj);

    TrObject obj1;
    obj1.name = "Y";
    obj1.rts = 0;
    obj1.wts = 0;
    obj1.value = "1";
    objectMng->addObject(obj1);

    objectMng->init();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
