TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++14

LIBS += -lboost_system
LIBS += -lboost_thread
LIBS += -lpthread
LIBS += -lcryptopp

SOURCES += main.cpp \
    message_base.cpp \
    message.cpp \
    core.cpp \
    peer.cpp \
    ui.cpp \
    library.cpp \
    glob.cpp

HEADERS += \
    glob.h \
    message_base.h \
    message.h \
    core.h \
    peer.h \
    ui.h \
    library.h

