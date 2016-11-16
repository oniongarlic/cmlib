QT += gui

CONFIG += mobility
MOBILITY += multimedia

CONFIG +=link_pkgconfig
PKGCONFIG += libsidplayfp libmodplug
LIBS+= -lasap

SOURCES += main.cpp audiooutput.cpp \
    cmbaseaudiosource.cpp \
    cmsidaudiosource.cpp \
    cmmodplugaudiosource.cpp \
    cmsapaudiosource.cpp
HEADERS += audiooutput.h \
    cmbaseaudiosource.h \
    cmsidaudiosource.h \
    cmmodplugaudiosource.h \
    cmsapaudiosource.h
