TEMPLATE = subdirs

SUBDIRS+= src
linux:SUBDIRS += tests

#Makes sure that 'src' is compiled before 'tests'
CONFIG += ordered
