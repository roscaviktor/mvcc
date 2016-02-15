TEMPLATE = app

QT += qml quick sql
CONFIG += c++11

TARGET = MVCCCluster

SOURCES += main.cpp \
    transactionmanager.cpp \
    datamanager.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    types.h \
    global.h \
    transactionmanager.h \
    datamanager.h
