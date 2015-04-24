TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += $$PWD/../../libs/cryptopp
INCLUDEPATH += $$PWD/../../libs/sqlite
INCLUDEPATH += $$PWD/../../libs

DEFINES += CRYPTOPP_EXPORTS

CONFIG += staticlib static

#LIBS += -L$$PWD/../../libs
LIBS += -lwsock32
LIBS += -lws2_32
LIBS += $$PWD/../../libs/libcryptopp.a
LIBS += $$PWD/../../libs/sqlite3.a
LIBS += $$PWD/../../libs/libboost_system-mgw49-1_58.a
LIBS += $$PWD/../../libs/libboost_filesystem-mgw49-1_58.dll.a
LIBS += $$PWD/../../libs/libboost_thread-mgw49-mt-1_58.a

INCLUDEPATH += $$PWD/../shared

#LIBS += -lboost_system
#LIBS += -lboost_thread
#LIBS += -lpthread
#LIBS += -lcryptopp
#LIBS += -lreadline

SOURCES += main.cpp \
    ui.cpp \
    connection.cpp \
    ../shared/common.cpp \
    ../shared/message.cpp \
    ../shared/message_base.cpp \
    ../shared/connection_initiator_base.cpp

HEADERS += \
    ui.h \
    connection.h \
    ../shared/common.h \
    ../shared/glob.h \
    ../shared/message.h \
    ../shared/message_base.h \
    ../shared/connection_initiator_base.h \
    ../shared/connection_base.h

