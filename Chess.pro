TEMPLATE = app

CONFIG += c++17
CONFIG += CONSOLE

QT += qml quick quickcontrols2 widgets

SOURCES += src/main.cpp \
    src/logic.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

HEADERS += \
    src/logic.h
