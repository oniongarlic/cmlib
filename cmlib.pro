QT += gui

# DEFINES += FLAC_DECODER

lessThan(QT_MAJOR_VERSION, 5): {
CONFIG += mobility
MOBILITY += multimedia
QT += declarative
} else {
QT += qml quick
QT += multimedia
# CONFIG += c++11
}

INCLUDEPATH += sources sinks player decoders

OTHER_FILES += test/*.qml

SOURCES += test/main.cpp \
    sources/cmbaseaudiosource.cpp \
    sources/cmsidaudiosource.cpp \
    sources/cmmodplugaudiosource.cpp \
    sources/cmsapaudiosource.cpp \
    scanner/cmmediascanner.cpp \
    decoders/cmmediadecoder.cpp \
    player/cmmediaplayer.cpp \
    sinks/cmbaseaudiosink.cpp sinks/cmqtaudiosink.cpp \
    sinks/cmfileaudiosink.cpp \
    sinks/cmwavfileaudiosink.cpp \
    sinks/cmbasethreadedaudiosink.cpp \
    sources/cmsc68audiosource.cpp

HEADERS += \
    sources/cmbaseaudiosource.h \
    sources/cmsidaudiosource.h \
    sources/cmmodplugaudiosource.h \
    sources/cmsapaudiosource.h \
    scanner/cmmediascanner.h \
    decoders/cmmediadecoder.h \
    player/cmmediaplayer.h \
    sinks/cmbaseaudiosink.h sinks/cmqtaudiosink.h \
    sinks/cmfileaudiosink.h \
    sinks/cmwavfileaudiosink.h \
    sinks/cmbasethreadedaudiosink.h \
    sinks/cmplaybackthread_p.h \
    sources/cmsc68audiosource.h

unix:!qnx {
    CONFIG +=link_pkgconfig

    PKGCONFIG += flac++
    PKGCONFIG += libmodplug
    PKGCONFIG += libsidplayfp
    PKGCONFIG += sc68

    LIBS+= -lasap
    # DEFINES += ALSAAUDIO
    DEFINES += QTAUDIO
}

qnx:blackberry {
    DEFINES += QNXAUDIO

    HEADERS += sinks/cmqsaaudiosink.h
    SOURCES += sinks/cmqsaaudiosink.cpp

    LIBS += -laudio_manager
    LIBS += -L3party/lib -lsidplayfp -lmodplug -lasound -lsc68 -lfile68 -lunice68 ./3party/lib/libasap.a
#./3party/lib/libayfly.a ./3party/lib/libayemu.a

    INCLUDEPATH += 3party/include
}

#######################################################################
# Sinks

# ALSA
contains(DEFINES,ALSAAUDIO) {
    HEADERS += sinks/cmalsaaudiosink.h
    SOURCES += sinks/cmalsaaudiosink.cpp
    LIBS+=-lasound
}

#######################################################################
# Sources

# FLAC
contains(DEFINES,FLAC_DECODER) {
    HEADERS += sources/cmflacaudiosource.h
    SOURCES += sources/cmflacaudiosource.cpp
    LIBS += -lFLAC -lFLAC++
}

DISTFILES += \
    test/*.qml
