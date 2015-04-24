TEMPLATE = subdirs

SUBDIRS= src #tests

#Makes sure that 'src' is compiled before 'tests'
CONFIG += ordered

# When below line is uncommented, it will build for Linux,
#  otherwise in basic mode for testing (and currently Windows)
DEFINES += LINUX
