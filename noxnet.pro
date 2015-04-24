TEMPLATE = subdirs

SUBDIRS+= src
unix:SUBDIRS += tests

#Makes sure that 'src' is compiled before 'tests'
CONFIG += ordered
