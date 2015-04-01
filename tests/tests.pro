TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += \
    ../src

LIBS += -lunittest++
LIBS += -lcryptopp

SOURCES += main.cpp
