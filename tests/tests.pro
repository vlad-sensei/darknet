TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += \
    ../src/shared \
    ../src/daemon

LIBS += -lboost_system
LIBS += -lboost_filesystem
LIBS += -lboost_thread
LIBS += -lboost_filesystem
LIBS += -lpthread
LIBS += -lsqlite3
LIBS += -lunittest++
LIBS += -lssl
LIBS += -lcrypto

HEADERS += database_test.h\
    ../src/daemon/core.h \
    ../src/daemon/peer.h \
    ../src/daemon/ui.h \
    ../src/daemon/library.h \
    ../src/daemon/sqlite3_base.h \
    ../src/daemon/database.h \
    ../src/daemon/ui_client.h \
    ../src/daemon/inventory.h \
    ../src/shared/common.h \
    ../src/shared/connection_base.h \
    ../src/shared/connection_initiator_base.h \
    ../src/shared/glob.h \
    ../src/shared/message_base.h \
    ../src/shared/message.h \

SOURCES += main.cpp \
    database_test.cpp \
    ../src/daemon/core.cpp \
    ../src/daemon/peer.cpp \
    ../src/daemon/ui.cpp \
    ../src/daemon/library.cpp \
    ../src/daemon/sqlite3_base.cpp \
    ../src/daemon/database.cpp \
    ../src/daemon/ui_client.cpp \
    ../src/daemon/inventory.cpp \
    ../src/shared/common.cpp \
    ../src/shared/connection_initiator_base.cpp \
    ../src/shared/message_base.cpp \
    ../src/shared/message.cpp \
