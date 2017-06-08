INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

#QT += sql

HEADERS += \
    $$PWD/ffmpeg_wrapper.h \
    $$PWD/recorder_splash.h \
    $$PWD/common_types.h \
    $$PWD/config.h \
    $$PWD/threadrun.h

SOURCES += \
    $$PWD/ffmpeg_wrapper.cpp \
    $$PWD/recorder_splash.cpp \
    $$PWD/config.cpp
