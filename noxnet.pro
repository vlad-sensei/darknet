TEMPLATE = subdirs

SUBDIRS= src tests

#Makes sure that 'src' is compiled before 'tests'
CONFIG += ordered

unix|win32: LIBS += -lncurses
