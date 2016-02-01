TEMPLATE = app

QT += qml quick sql
CONFIG += c++11

TARGET = MVCCCluster

SOURCES += main.cpp \
    transactionmng.cpp \
    objectmng.cpp \
    global.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    transactionmng.h \
    types.h \
    objectmng.h \
    global.h
