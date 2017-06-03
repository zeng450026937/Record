#-------------------------------------------------
#
# Project created by QtCreator 2016-09-12T15:08:45
#
#-------------------------------------------------

QT       += core gui av sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Recorder
TEMPLATE = app

include(scenes/scenes.pri)
include(model/model.pri)
include(common/common.pri)
include(service/service.pri)
include(notification/notification.pri)

DEFINES += \

INCLUDEPATH += \
../ \

SOURCES += main.cpp\
        recorder.cpp \
    recorder_shared.cpp

HEADERS  += recorder.h \
    recorder_shared.h

FORMS    += recorder.ui


RESOURCES += \
    resource.qrc
