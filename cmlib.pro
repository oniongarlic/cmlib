QT += gui
QT += declarative

CONFIG += mobility
MOBILITY += multimedia

#DEFINES += FLAC_DECODER

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
    sinks/cmwavfileaudiosink.cpp \
    sinks/cmbasethreadedaudiosink.cpp

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
    sinks/cmwavfileaudiosink.h \
    sinks/cmbasethreadedaudiosink.h \
    sinks/cmplaybackthread_p.h

contains(DEFINES,FLAC_DECODER) {
    HEADERS += sources/cmflacaudiosource.h
    SOURCES += sources/cmflacaudiosource.cpp
}

unix:!qnx {
CONFIG +=link_pkgconfig
PKGCONFIG += libsidplayfp libmodplug flac++
LIBS+= -lasap
    DEFINES += ALSAAUDIO
    HEADERS += sinks/cmalsaaudiosink.h
    SOURCES += sinks/cmalsaaudiosink.cpp
    LIBS+=-lasound
}

qnx:blackberry {
    DEFINES += QNXAUDIO

    HEADERS += sinks/cmqsaaudiosink.h
    SOURCES += sinks/cmqsaaudiosink.cpp

    LIBS += -laudio_manager
    LIBS += -L3party/lib -lsidplayfp -lmodplug -lasound -lsc68 -lfile68 -lunice68 ./3party/lib/libasap.a

contains(DEFINES,FLAC_DECODER) {
    LIBS += -lFLAC -lFLAC++
}

#../lib/libayfly.a ../lib/libayemu.a
    INCLUDEPATH += 3party/include
}
