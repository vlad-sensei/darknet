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
LIBS += -lcryptopp
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
    common.h \
    glob.h \
    message.h \
    ..\shared\message_base.h \
    connection_initiator_base.h \
    connection_base.h \
    endian/conversion.hpp
