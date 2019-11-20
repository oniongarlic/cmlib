QT += gui sql

ANDROID_SYSROOT=/home/milang/Android/toolchain-ndk-android-24/sysroot

lessThan(QT_MAJOR_VERSION, 5): {
    CONFIG += mobility
    MOBILITY += multimedia
    QT += declarative
} else {
    QT += qml quick multimedia
    # For Tray icon
    QT += widgets
    CONFIG += c++11
}

INCLUDEPATH += sources sinks player decoders

OTHER_FILES += test/*.qml test/qtquick1/*.qml test/qtquick2/*.qml

SOURCES += \
    sources/cmbaseaudiosource.cpp \
    scanner/cmmediascanner.cpp \
    decoders/cmmediadecoder.cpp \
    player/cmmediaplayer.cpp \
    sinks/cmbaseaudiosink.cpp sinks/cmqtaudiosink.cpp \
    sinks/cmfileaudiosink.cpp \
    sinks/cmwavfileaudiosink.cpp \
    sinks/cmbasethreadedaudiosink.cpp \    
    scanner/cmlibrarymodel.cpp

HEADERS += \
    sources/cmbaseaudiosource.h \
    scanner/cmmediascanner.h \
    decoders/cmmediadecoder.h \
    player/cmmediaplayer.h \
    sinks/cmbaseaudiosink.h sinks/cmqtaudiosink.h \
    sinks/cmfileaudiosink.h \
    sinks/cmwavfileaudiosink.h \
    sinks/cmbasethreadedaudiosink.h \
    sinks/cmplaybackthread_p.h \
    scanner/cmlibrarymodel.h

SOURCES += test/main.cpp

unix {
    DEFINES += ALSAAUDIO
    CONFIG +=link_pkgconfig
}

darwin {
    # PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/
    LIBS += -L/usr/local/lib
    INCLUDEPATH += /usr/local/include
    DEFINES += QTAUDIO
    DEFINES -= ALSAAUDIO
}

unix:!qnx:!android {

packagesExist(flac++) {
    DEFINES += FLAC_DECODER
    PKGCONFIG += flac++
}

packagesExist(opusfile) {
    DEFINES += OPUS_DECODER
    PKGCONFIG += opusfile
}

packagesExist(sc68) {
    DEFINES += SC68_DECODER
    PKGCONFIG += sc68
}

packagesExist(libsidplayfp) {
    DEFINES += SID_DECODER
    PKGCONFIG += libsidplayfp
}

lessThan(QT_MAJOR_VERSION, 5): {
    packagesExist(libmodplug) {
        DEFINES += MOD_DECODER
        PKGCONFIG += libmodplug
    }
} else {
    packagesExist(libopenmpt) {
        DEFINES += MTP_DECODER
        PKGCONFIG += libopenmpt
        CONFIG += c++11
    }
}
    DEFINES += AY_DECODER

    # DEFINES += ALSAAUDIO
    DEFINES += QTAUDIO
} else {
    # Non-pkg-config environments

    # DEFINES += FLAC_DECODER
    # DEFINES += SAP_DECODER
    DEFINES += SID_DECODER
    # DEFINES += MOD_DECODER
    DEFINES += MTP_DECODER
    DEFINES += AY_DECODER
}

qnx:blackberry {
    DEFINES += QNXAUDIO

    LIBS += -L3rdparty/qnx-armv7/lib
    LIBS += -lsidplayfp
    LIBS += -lmodplug
    LIBS += -lsc68 -lfile68 -lunice68
    LIBS += ./3rdparty/qnx-armv7/lib/libasap.a

#./3party/lib/libayfly.a ./3party/lib/libayemu.a

    INCLUDEPATH += 3rdparty/include
}

android {
    #LIBS += -L3rdparty/android-armv7/lib
    LIBS += -L$$ANDROID_SYSROOT/lib
    INCLUDEPATH += $$ANDROID_SYSROOT/include

    LIBS += -lsidplayfp
    LIBS += -lmodplug
    # LIBS += -lsc68 -lfile68 -lunice68
    # LIBS += ./3rdparty/android-armv7/lib/libasap.a

    DEFINES += QTAUDIO

    contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
        ANDROID_EXTRA_LIBS = $$ANDROID_SYSROOT/lib/libsidplayfp.so \
    $$ANDROID_SYSROOT/lib/libmodplug.so
    }
}


#######################################################################
# Sinks

# ALSA
contains(DEFINES,ALSAAUDIO) {
    HEADERS += sinks/cmalsaaudiosink.h
    SOURCES += sinks/cmalsaaudiosink.cpp
    LIBS+=-lasound
}

# QNX
contains(DEFINES,QNXAUDIO) {
    HEADERS += sinks/cmqsaaudiosink.h
    SOURCES += sinks/cmqsaaudiosink.cpp
    LIBS += -lasound -laudio_manager
}

#######################################################################
# Sources

# FLAC
# https://xiph.org/flac/
contains(DEFINES,FLAC_DECODER) {
    HEADERS += sources/cmflacaudiosource.h
    SOURCES += sources/cmflacaudiosource.cpp
    LIBS += -lFLAC -lFLAC++
}

# Modplug
# https://github.com/Konstanty/libmodplug
contains(DEFINES,MOD_DECODER) {
    HEADERS += sources/cmmodplugaudiosource.h
    SOURCES += sources/cmmodplugaudiosource.cpp
}

# OpenMTP
# https://lib.openmpt.org/libopenmpt/
contains(DEFINES,MTP_DECODER) {
    HEADERS += sources/cmopenmtpaudiosource.h
    SOURCES += sources/cmopenmtpaudiosource.cpp
    # LIBS += -lopenmtp
}

# SID
# https://sourceforge.net/p/sidplay-residfp/wiki/Home/
contains(DEFINES,SID_DECODER) {
    HEADERS += sources/cmsidaudiosource.h
    SOURCES += sources/cmsidaudiosource.cpp
}

# SC68
# https://sourceforge.net/projects/sc68/
contains(DEFINES,SC68_DECODER) {
    HEADERS += sources/cmsc68audiosource.h
    SOURCES += sources/cmsc68audiosource.cpp
}

# ASAP
# http://asap.sourceforge.net/
contains(DEFINES,SAP_DECODER) {
    HEADERS += sources/cmsapaudiosource.h
    SOURCES += sources/cmsapaudiosource.cpp
    LIBS+= -lasap
}

# AYEMU
# https://github.com/asashnov/libayemu
contains(DEFINES,AY_DECODER) {
    HEADERS += sources/cmayaudiosource.h
    SOURCES += sources/cmayaudiosource.cpp
    LIBS += -L/usr/local/lib -layemu
}

# Opus
#
contains(DEFINES,OPUS_DECODER) {
    HEADERS += sources/cmopusaudiosource.h
    SOURCES += sources/cmopusaudiosource.cpp
}

#######################################################################

DISTFILES += \
    test/*.qml \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

RESOURCES += \
    test/test.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
