TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++14

win32{
    QMAKE_LFLAGS += -static-libgcc

    INCLUDEPATH += $$PWD/../../libs/sqlite
    INCLUDEPATH += $$PWD/../../libs

    LIBS += -lwsock32
    LIBS += -lws2_32
    LIBS += $$PWD/../../libs/sqlite3.dll
    LIBS += $$PWD/../../libs/libboost_system-mgw49-1_58.dll
    LIBS += $$PWD/../../libs/libboost_filesystem-mgw49-1_58.dll
    LIBS += $$PWD/../../libs/libboost_thread-mgw49-mt-1_58.dll
    LIBS += $$PWD/../../libs/ssleay32.dll
    LIBS += $$PWD/../../libs/libeay32.dll
}
linux{
    LIBS += -lboost_system
    LIBS += -lboost_filesystem
    LIBS += -lboost_thread
    LIBS += -lpthread
    LIBS += -lsqlite3
    LIBS += -lreadline
    LIBS += -lssl
    LIBS += -lcrypto
}

INCLUDEPATH += $$PWD/../shared

SOURCES += main.cpp \
    core.cpp \
    peer.cpp \
    ui.cpp \
    library.cpp \
    sqlite3_base.cpp \
    database.cpp \
    ui_client.cpp \
    inventory.cpp \
    ../shared/common.cpp \
    ../shared/connection_initiator_base.cpp \
    ../shared/message_base.cpp \
    ../shared/message.cpp \

HEADERS += \
    core.h \
    peer.h \
    ui.h \
    library.h \
    sqlite3_base.h \
    database.h \
    ui_client.h \
    inventory.h \
    ../shared/common.h \
    ../shared/connection_base.h \
    ../shared/connection_initiator_base.h \
    ../shared/glob.h \
    ../shared/message_base.h \
    ../shared/message.h \
