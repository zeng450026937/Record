INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/asio
INCLUDEPATH += $$PWD/websocketpp
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/service_thread.h \
    $$PWD/messager/message_base.h \
    $$PWD/messager/message_base_private.h \
    $$PWD/messager/network_info.h \
    $$PWD/messager/websocketclient.h \
    $$PWD/messager/binary_reader.h \
    $$PWD/service_thread_private.h \
    $$PWD/conf_service_impl.h \
    $$PWD/user_service_impl.h \
    $$PWD/storage/conferencedatabase_impl.h \
    $$PWD/storage/database_impl.h \
    $$PWD/storage/device_database_impl.h \
    $$PWD/storage/filedatabase_impl.h \
    $$PWD/storage/markdatabase_impl.h \
    $$PWD/storage/shareddata.h \
    $$PWD/storage/templatedatabase_impl.h \
    $$PWD/storage/user_database_impl.h \
    $$PWD/storage/include/conferencedatabase.h \
    $$PWD/storage/include/data_base.h \
    $$PWD/storage/include/device_database.h \
    $$PWD/storage/include/filedatabase.h \
    $$PWD/storage/include/markdatabase.h \
    $$PWD/storage/include/templatedatabase.h \
    $$PWD/storage/include/user_database.h \
    $$PWD/storage/include/clip_file_database.h \
    $$PWD/storage/clip_file_database_impl.h \
    $$PWD/storage/include/personal_database.h \
    $$PWD/storage/personal_database_impl.h \
    $$PWD/account/account_center.h \
    $$PWD/white_list.h \
    $$PWD/messager/wavheader.h \
    $$PWD/command/ConferenceMode.h \
    $$PWD/command/PersonalMode.h \
    $$PWD/messager/CommandBase.h \
    $$PWD/command/RecordDownloadReceiver.h \
    $$PWD/command/RecordDownloadService.h \
    $$PWD/storage/download_database.h \
    $$PWD/command/LoginControl.h \
    $$PWD/command/RecordControl.h


SOURCES += \
    $$PWD/service_thread.cpp \
    $$PWD/messager/message_base.cpp \
    $$PWD/messager/message_base_private.cpp \
    $$PWD/messager/network_info.cpp \
    $$PWD/messager/binary_reader.cpp \
    $$PWD/messager/websocketclient.cpp \
    $$PWD/service_thread_private.cpp \
    $$PWD/conf_service_impl.cpp \
    $$PWD/user_service_impl.cpp \
    $$PWD/storage/conferencedatabase_impl.cpp \
    $$PWD/storage/database_impl.cpp \
    $$PWD/storage/device_database_impl.cpp \
    $$PWD/storage/filedatabase_impl.cpp \
    $$PWD/storage/markdatabase_impl.cpp \
    $$PWD/storage/shareddata.cpp \
    $$PWD/storage/templatedatabase_impl.cpp \
    $$PWD/storage/user_database_impl.cpp \
    $$PWD/storage/clip_file_database_impl.cpp \
    $$PWD/storage/personal_database_impl.cpp \
    $$PWD/account/account_center.cpp \
    $$PWD/messager/wavheader.cpp \
    $$PWD/command/ConferenceMode.cpp \
    $$PWD/command/PersonalMode.cpp \
    $$PWD/messager/CommandBase.cpp \
    $$PWD/command/RecordDownloadReceiver.cpp \
    $$PWD/command/RecordDownloadService.cpp \
    $$PWD/storage/download_database.cpp \
    $$PWD/command/LoginControl.cpp \
    $$PWD/command/RecordControl.cpp

