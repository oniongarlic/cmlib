QT += gui
QT += declarative

CONFIG += mobility
MOBILITY += multimedia

CONFIG +=link_pkgconfig
PKGCONFIG += libsidplayfp libmodplug
LIBS+= -lasap

INCLUDEPATH += sources

OTHER_FILES += main.qml Button.qml

SOURCES += test/main.cpp test/audiooutput.cpp \
    sources/cmbaseaudiosource.cpp \
    sources/cmsidaudiosource.cpp \
    sources/cmmodplugaudiosource.cpp \
    sources/cmsapaudiosource.cpp \
    sources/cmmediascanner.cpp \
    sources/cmmediadecoder.cpp
HEADERS += test/audiooutput.h \
    sources/cmbaseaudiosource.h \
    sources/cmsidaudiosource.h \
    sources/cmmodplugaudiosource.h \
    sources/cmsapaudiosource.h \
    sources/cmmediascanner.h \
    sources/cmmediadecoder.h
