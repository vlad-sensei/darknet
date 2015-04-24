TEMPLATE = subdirs

SUBDIRS += daemon
unix:SUBDIRS += client_console

CONFIG += ordered
