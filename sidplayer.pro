QT += gui
QT += declarative
CONFIG += mobility
MOBILITY += multimedia

CONFIG +=link_pkgconfig
PKGCONFIG += libsidplayfp

SOURCES += main.cpp audiooutput.cpp
HEADERS += audiooutput.h
