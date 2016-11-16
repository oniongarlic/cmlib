QT += gui

CONFIG += mobility
MOBILITY += multimedia

CONFIG +=link_pkgconfig
PKGCONFIG += libsidplayfp libmodplug
LIBS+= -lasap

INCLUDEPATH += sources

SOURCES += test/main.cpp test/audiooutput.cpp \
    sources/cmbaseaudiosource.cpp \
    sources/cmsidaudiosource.cpp \
    sources/cmmodplugaudiosource.cpp \
    sources/cmsapaudiosource.cpp
HEADERS += test/audiooutput.h \
    sources/cmbaseaudiosource.h \
    sources/cmsidaudiosource.h \
    sources/cmmodplugaudiosource.h \
    sources/cmsapaudiosource.h
