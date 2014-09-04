# ensure one "debug_and_release" in CONFIG, for clarity...
debug_and_release {
    CONFIG -= debug_and_release
    CONFIG += debug_and_release
}

# ensure one "debug" or "release" in CONFIG so they can be used as
#   conditionals instead of writing "CONFIG(debug, debug|release)"...
CONFIG(debug, debug|release) {
    CONFIG -= debug release
    CONFIG += debug
}

CONFIG(release, debug|release) {
    CONFIG -= debug release
    CONFIG += release
}


contains(QT_ARCH, "i386") {

    debug {
        BUILD_DIR = "temp/build32/uwebkit/debug"
    }

    release {
        BUILD_DIR = "temp/build32/uwebkit/release"
    }

    win32 {
    DEFINES += PSAPI_VERSION=1 WINVER=0x501 WIN32_WINNT=0x501
    }

    CONFIG -= x86 x86_64
    CONFIG += x86

}

contains(QT_ARCH, "x86_64") {

    debug {
        BUILD_DIR = "temp/build64/uwebkit/debug"
    }

    release {
        BUILD_DIR = "temp/build64/uwebkit/release"
    }

    CONFIG -= x86 x86_64
    CONFIG += x86_64

}

win32 {
    BUILD_DIR = $$absolute_path($$BUILD_DIR)
    DESTDIR = $$BUILD_DIR
}

macx {
    DESTDIR = $$absolute_path($$BUILD_DIR)
}

OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR = $$DESTDIR/moc
RCC_DIR = $$DESTDIR/qrc
UI_DIR = $$DESTDIR/ui

INCLUDEPATH += ../ ../ThirdParty ../ThirdParty/poco/Foundation/include ../ThirdParty/jansson

# for jansson
DEFINES += HAVE_STDINT_H

win32 {
    DEFINES += POCO_STATIC POCO_NO_AUTOMATIC_LIBS BOOST_DATE_TIME_NO_LIB
    LIBS += Iphlpapi.lib

    release {
        #DEBUG info in release build on windows
        #Need to make sure pdb is in appropriate spot too
        #QMAKE_CXXFLAGS_RELEASE += /Zi
        #QMAKE_LFLAGS_RELEASE += /DEBUG
    }

}
