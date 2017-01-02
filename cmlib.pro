QT += gui
QT += declarative

CONFIG += mobility
MOBILITY += multimedia

CONFIG +=link_pkgconfig
PKGCONFIG += libsidplayfp libmodplug
LIBS+= -lasap

INCLUDEPATH += sources sinks player decoders

OTHER_FILES += test/main.qml test/Button.qml

SOURCES += test/main.cpp \
    sources/cmbaseaudiosource.cpp \
    sources/cmsidaudiosource.cpp \
    sources/cmmodplugaudiosource.cpp \
    sources/cmsapaudiosource.cpp \
    sources/cmmediascanner.cpp \
    decoders/cmmediadecoder.cpp \
    sinks/cmbaseaudiosink.cpp sinks/cmqtaudiosink.cpp \
    player/cmmediaplayer.cpp \
    sinks/cmfileaudiosink.cpp \
    sinks/cmwavfileaudiosink.cpp

HEADERS += \
    sources/cmbaseaudiosource.h \
    sources/cmsidaudiosource.h \
    sources/cmmodplugaudiosource.h \
    sources/cmsapaudiosource.h \
    sources/cmmediascanner.h \
    decoders/cmmediadecoder.h \
    sinks/cmbaseaudiosink.h sinks/cmqtaudiosink.h \
    player/cmmediaplayer.h \
    sinks/cmfileaudiosink.h \
    sinks/cmwavfileaudiosink.h
