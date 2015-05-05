TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += $$PWD/../shared
INCLUDEPATH += $$PWD/../../libs

LIBS += -lboost_system
LIBS += -lboost_filesystem
LIBS += -lboost_thread
LIBS += -lpthread
LIBS += -lsqlite3
LIBS += -lcryptopp
LIBS += -lssl
LIBS += -lcrypto

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
    common.h \
    connection_base.h \
    connection_initiator_base.h \
    glob.h \
    ..\shared\message_base.h \
    message.h
