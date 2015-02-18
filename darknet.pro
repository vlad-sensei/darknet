TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lboost_system
LIBS += -lboost_thread
LIBS += -lpthread

SOURCES += main.cpp \
    message_base.cpp \
    message.cpp \
    core.cpp \
    peer.cpp \
    ui.cpp \
    library.cpp

HEADERS += \
    glob.h \
    message_base.h \
    message.h \
    core.h \
    peer.h \
    ui.h \
    library.h

