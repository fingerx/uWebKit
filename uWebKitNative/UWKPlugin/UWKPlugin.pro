include (../UWKNative.pri)

TEMPLATE = lib
CONFIG += plugin

macx {

    CONFIG += plugin_bundle
    QMAKE_BUNDLE_EXTENSION = .bundle
    QMAKE_BUNDLE_LOCATION = Contents/MacOS
    
}

CONFIG -= qt

HEADERS += \
    uwk_plugin.h \
    uwk_unity_keys.h \
    uwk_unity_render.h \
    uwk_unity_beta.h

SOURCES += \
    uwk_plugin.cpp \
    uwk_unity_keys.cpp \
    uwk_unity_texture.cpp \
    uwk_unity_beta.cpp \
    uwk_unity_render.cpp

win32 {

    HEADERS +=
    SOURCES +=
    PRE_TARGETDEPS += $$BUILD_DIR/UWKCommon.lib
    LIBS += $$BUILD_DIR/UWKCommon.lib

    target.path += $$PWD/../../uWebKit/Assets/Plugins/x86

}

macx {

    LIBS += $$DESTDIR/libUWKCommon.a "-framework IOSurface" "-framework OpenGL"
    PRE_TARGETDEPS += $$DESTDIR/libUWKCommon.a

    target.path += $$PWD/../../uWebKit/Assets/Plugins
}


INSTALLS += target
