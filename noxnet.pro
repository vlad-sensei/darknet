TEMPLATE = subdirs

SUBDIRS= src# tests

#Makes sure that 'src' is compiled before 'tests'
CONFIG += ordered
