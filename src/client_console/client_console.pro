TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += $$PWD/../shared

win32{
    QMAKE_LFLAGS += -static-libgcc

    INCLUDEPATH += $$PWD/../../libs/sqlite
    INCLUDEPATH += $$PWD/../../libs

    LIBS += -lwsock32
    LIBS += -lws2_32
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
    LIBS += -lncurses
    LIBS += -lssl
    LIBS += -lcrypto
}

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
