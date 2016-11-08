QT += gui

CONFIG += mobility
MOBILITY += multimedia

CONFIG +=link_pkgconfig
PKGCONFIG += libsidplayfp

SOURCES += main.cpp audiooutput.cpp \
    cmbaseaudiosource.cpp \
    cmsidaudiosource.cpp
HEADERS += audiooutput.h \
    cmbaseaudiosource.h \
    cmsidaudiosource.h
