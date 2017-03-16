QT += gui sql

# DEFINES += FLAC_DECODER
# DEFINES += SAP_DECODER
DEFINES += SID_DECODER
DEFINES += MOD_DECODER

ANDROID_SYSROOT=/home/milang/Android/toolchain-ndk-android-24/sysroot

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

unix:!qnx:!android {
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

    LIBS += -lasound -laudio_manager

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

#######################################################################
# Sources

# FLAC
contains(DEFINES,FLAC_DECODER) {
    HEADERS += sources/cmflacaudiosource.h
    SOURCES += sources/cmflacaudiosource.cpp
    LIBS += -lFLAC -lFLAC++
}

# Modplug
contains(DEFINES,MOD_DECODER) {
    HEADERS += sources/cmmodplugaudiosource.h
    SOURCES += sources/cmmodplugaudiosource.cpp
}

# SID
contains(DEFINES,SID_DECODER) {
    HEADERS += sources/cmsidaudiosource.h
    SOURCES += sources/cmsidaudiosource.cpp
}

# SC68
contains(DEFINES,SC68_DECODER) {
    HEADERS += sources/cmsc68audiosource.h
    SOURCES += sources/cmsc68audiosource.cpp
}

# ASAP
contains(DEFINES,SAP_DECODER) {
    HEADERS += sources/cmsapaudiosource.h
    SOURCES += sources/cmsapaudiosource.cpp
}

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
