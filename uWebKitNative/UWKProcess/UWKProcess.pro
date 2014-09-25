include (../UWKNative.pri)

QT += widgets webkitwidgets network

CONFIG += qt

DEFINES += QT_NO_CAST_FROM_ASCII \
           QT_NO_CAST_TO_ASCII

HEADERS += uwk_qt_utilities.h \
           uwk_application.h \
           uwk_webpage.h \
           uwk_webview.h \
           uwk_engine.h \
           uwk_gpusurface.h \
           uwk_networkaccessmanager.h \
           uwk_networkcookiejar.h \
           uwk_jsbridge_qt.h \
           uwk_javascript_embedded.h

SOURCES += main.cpp \
           uwk_qt_utilities.cpp \
           uwk_application.cpp \
           uwk_webpage.cpp \
           uwk_webview.cpp \
           uwk_engine.cpp \
           uwk_networkaccessmanager.cpp \
           uwk_networkcookiejar.cpp \
           uwk_gpusurface.cpp \
           uwk_jsbridge_qt.cpp \
           uwk_javascript_embedded.cpp


win32 {

    PRE_TARGETDEPS += $$BUILD_DIR/UWKCommon.lib
    LIBS += $$BUILD_DIR/UWKCommon.lib

    HEADERS += uwk_gpusurface_sharedmemory.h uwk_gpusurface_d3d9.h uwk_gpusurface_d3d11.h uwk_gpusurface_d3d11_sharedmemory.h
    SOURCES += uwk_gpusurface_sharedmemory.cpp uwk_gpusurface_d3d9.cpp uwk_gpusurface_d3d11.cpp uwk_gpusurface_d3d11_sharedmemory.cpp

    target.path += $$PWD/../../uWebKit/Assets/StreamingAssets/uWebKit/Windows/x86

}

macx {

    HEADERS += uwk_gpusurface_mac.h

    OBJECTIVE_SOURCES += uwk_gpusurface_mac.mm

    #IMPORTANT NOTE: we get hangs if the plist which specifies
    #<key>LSUIElement</key> <string>1</string> isn't used

    QMAKE_INFO_PLIST = $$PWD/Info.plist

    LIBS += $$DESTDIR/libUWKCommon.a "-framework AppKit" "-framework IOSurface" "-framework OpenGL"
    PRE_TARGETDEPS += $$DESTDIR/libUWKCommon.a

    target.path += $$PWD/../../uWebKit/Assets/StreamingAssets/uWebKit/Mac/x86

    # do not have shadow build set in QtCreator or this is never called?
    QMAKE_POST_LINK += "$$QT_BIN_PATH/macdeployqt $$DESTDIR/UWKProcess.app"
}

INSTALLS += target


