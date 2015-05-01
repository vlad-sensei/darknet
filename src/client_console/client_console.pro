TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += $$PWD/../shared

LIBS += -lboost_system
LIBS += -lboost_filesystem
LIBS += -lboost_thread
LIBS += -lpthread
LIBS += -lssl
LIBS += -lcrypto

linux: LIBS += -lncurses

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

