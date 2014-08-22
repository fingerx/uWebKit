cmake_minimum_required(VERSION 2.8)

project (UWKPlugin)

include (CheckIncludeFiles)

include_directories( ${CMAKE_SOURCE_DIR} 
                     ${CMAKE_SOURCE_DIR}/ThirdParty 
                     ${CMAKE_SOURCE_DIR}/ThirdParty/poco/Foundation/include 
                     ${CMAKE_SOURCE_DIR}/ThirdParty/jansson )


add_definitions(-DHAVE_STDINT_H -DPOCO_STATIC -DPOCO_NO_AUTOMATIC_LIBS -DBOOST_DATE_TIME_NO_LIB)

set ( UWK_PLUGIN_SOURCE UWKPlugin/uwk_plugin.cpp UWKPlugin/uwk_unity_keys.cpp 
                        UWKPlugin/uwk_unity_beta.cpp UWKPlugin/uwk_unity_render.cpp
                        UWKPlugin/uwk_unity_texture.cpp )

file(GLOB JANSSON_SOURCE ThirdParty/jansson/*.c)

if (APPLE)

    set (CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk)
    set (CMAKE_OSX_DEPLOYMENT_TARGET 10.6)
    set (CMAKE_OSX_ARCHITECTURES "x86_64;i386")

    set (UWKCOMMON_SOURCE UWKCommon/uwk_renderer_gl.cpp)

else()

    add_definitions( -DUNICODE -DWIN32 -DHAVE_STDINT_H)

    #-c -nologo -Zm200 -Zc:wchar_t -O2 -MD -GR -GL -W3 -w34100 -w34189 -EHsc

    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /arch:SSE2")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /arch:SSE2")    
    #SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    
    STRING(REPLACE "/GX" "" CMAKE_CXX_FLAGS_INIT ${CMAKE_CXX_FLAGS_INIT})
    STRING(REPLACE "/GX" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

    #statically link
    FOREACH(flag_var CMAKE_CXX_FLAGS 
    CMAKE_CXX_FLAGS_DEBUG 
    CMAKE_CXX_FLAGS_RELEASE 
    CMAKE_CXX_FLAGS_MINSIZEREL 
    CMAKE_CXX_FLAGS_RELWITHDEBINFO
    CMAKE_C_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELEASE
    CMAKE_C_FLAGS_MINSIZEREL 
    CMAKE_C_FLAGS_RELWITHDEBINFO )
        IF(${flag_var} MATCHES "/MD")
            STRING(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        ENDIF(${flag_var} MATCHES "/MD")
        
        IF(${flag_var} MATCHES "/MDd")
            STRING(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
        ENDIF(${flag_var} MATCHES "/MDd")

        SET(${flag_var} "${${flag_var}} /MP${LOOM_BUILD_NUMCORES}")

    ENDFOREACH(flag_var)       

    set (UWKCOMMON_SOURCE UWKCommon/uwk_renderer_sharedmemory.cpp UWKCommon/uwk_renderer_d3d9.cpp UWKCommon/uwk_renderer_d3d11.cpp)
    
endif()


set ( UWKCOMMON_SOURCE ${JANSSON_SOURCE}  
	${UWKCOMMON_SOURCE}   
    UWKCommon/uwk_config.cpp
    UWKCommon/uwk_browser.cpp 
    UWKCommon/uwk_keyboard.cpp 
    UWKCommon/uwk_renderer.cpp
    UWKCommon/uwk_server.cpp
    UWKCommon/uwk_log.cpp    
    UWKCommon/uwk_error.cpp
    UWKCommon/uwk_message.cpp
    UWKCommon/uwk_process_utils.cpp
    UWKCommon/uwk_process_common.cpp
    UWKCommon/uwk_process_server.cpp
    UWKCommon/uwk_process_client.cpp
    UWKCommon/uwk_processdb.cpp   
    ThirdParty/sqlite/sqlite3.c 
    ThirdParty/poco/Foundation/src/ASCIIEncoding.cpp 
    ThirdParty/poco/Foundation/src/Ascii.cpp 
    ThirdParty/poco/Foundation/src/AtomicCounter.cpp 
    ThirdParty/poco/Foundation/src/AbstractObserver.cpp 
    ThirdParty/poco/Foundation/src/ActiveDispatcher.cpp 
    ThirdParty/poco/Foundation/src/ArchiveStrategy.cpp 
    ThirdParty/poco/Foundation/src/AsyncChannel.cpp 
    ThirdParty/poco/Foundation/src/Base64Decoder.cpp 
    ThirdParty/poco/Foundation/src/Base64Encoder.cpp 
    ThirdParty/poco/Foundation/src/Base32Decoder.cpp 
    ThirdParty/poco/Foundation/src/Base32Encoder.cpp 
    ThirdParty/poco/Foundation/src/BinaryReader.cpp 
    ThirdParty/poco/Foundation/src/BinaryWriter.cpp 
    ThirdParty/poco/Foundation/src/Bugcheck.cpp 
    ThirdParty/poco/Foundation/src/ByteOrder.cpp 
    ThirdParty/poco/Foundation/src/Channel.cpp 
    ThirdParty/poco/Foundation/src/Checksum.cpp 
    ThirdParty/poco/Foundation/src/Clock.cpp 
    ThirdParty/poco/Foundation/src/Condition.cpp 
    ThirdParty/poco/Foundation/src/Configurable.cpp 
    ThirdParty/poco/Foundation/src/ConsoleChannel.cpp 
    ThirdParty/poco/Foundation/src/CountingStream.cpp 
    ThirdParty/poco/Foundation/src/DateTime.cpp 
    ThirdParty/poco/Foundation/src/DateTimeFormat.cpp 
    ThirdParty/poco/Foundation/src/DateTimeFormatter.cpp 
    ThirdParty/poco/Foundation/src/DateTimeParser.cpp 
    ThirdParty/poco/Foundation/src/Debugger.cpp 
    ThirdParty/poco/Foundation/src/DeflatingStream.cpp 
    ThirdParty/poco/Foundation/src/DigestEngine.cpp 
    ThirdParty/poco/Foundation/src/DigestStream.cpp 
    ThirdParty/poco/Foundation/src/DirectoryIterator.cpp 
    ThirdParty/poco/Foundation/src/DirectoryIteratorStrategy.cpp 
    ThirdParty/poco/Foundation/src/DirectoryWatcher.cpp 
    ThirdParty/poco/Foundation/src/Environment.cpp 
    ThirdParty/poco/Foundation/src/Error.cpp 
    ThirdParty/poco/Foundation/src/ErrorHandler.cpp 
    ThirdParty/poco/Foundation/src/Event.cpp 
    ThirdParty/poco/Foundation/src/EventArgs.cpp 
    ThirdParty/poco/Foundation/src/Exception.cpp 
    ThirdParty/poco/Foundation/src/FPEnvironment.cpp 
    ThirdParty/poco/Foundation/src/File.cpp 
    ThirdParty/poco/Foundation/src/FIFOBufferStream.cpp 
    ThirdParty/poco/Foundation/src/FileChannel.cpp 
    ThirdParty/poco/Foundation/src/FileStream.cpp 
    ThirdParty/poco/Foundation/src/FileStreamFactory.cpp 
    ThirdParty/poco/Foundation/src/Format.cpp 
    ThirdParty/poco/Foundation/src/Formatter.cpp 
    ThirdParty/poco/Foundation/src/FormattingChannel.cpp 
    ThirdParty/poco/Foundation/src/Glob.cpp 
    ThirdParty/poco/Foundation/src/Hash.cpp 
    ThirdParty/poco/Foundation/src/HashStatistic.cpp 
    ThirdParty/poco/Foundation/src/HexBinaryDecoder.cpp 
    ThirdParty/poco/Foundation/src/HexBinaryEncoder.cpp 
    ThirdParty/poco/Foundation/src/InflatingStream.cpp 
    ThirdParty/poco/Foundation/src/Latin1Encoding.cpp 
    ThirdParty/poco/Foundation/src/Latin2Encoding.cpp 
    ThirdParty/poco/Foundation/src/Latin9Encoding.cpp 
    ThirdParty/poco/Foundation/src/LineEndingConverter.cpp 
    ThirdParty/poco/Foundation/src/LocalDateTime.cpp 
    ThirdParty/poco/Foundation/src/LogFile.cpp 
    ThirdParty/poco/Foundation/src/LogStream.cpp 
    ThirdParty/poco/Foundation/src/Logger.cpp 
    ThirdParty/poco/Foundation/src/LoggingFactory.cpp 
    ThirdParty/poco/Foundation/src/LoggingRegistry.cpp 
    ThirdParty/poco/Foundation/src/MD4Engine.cpp 
    ThirdParty/poco/Foundation/src/MD5Engine.cpp 
    ThirdParty/poco/Foundation/src/Manifest.cpp 
    ThirdParty/poco/Foundation/src/MemoryPool.cpp 
    ThirdParty/poco/Foundation/src/MemoryStream.cpp 
    ThirdParty/poco/Foundation/src/Message.cpp 
    ThirdParty/poco/Foundation/src/Mutex.cpp 
    ThirdParty/poco/Foundation/src/NamedEvent.cpp 
    ThirdParty/poco/Foundation/src/NamedMutex.cpp 
    ThirdParty/poco/Foundation/src/NestedDiagnosticContext.cpp 
    ThirdParty/poco/Foundation/src/Notification.cpp 
    ThirdParty/poco/Foundation/src/NotificationCenter.cpp 
    ThirdParty/poco/Foundation/src/NotificationQueue.cpp 
    ThirdParty/poco/Foundation/src/TimedNotificationQueue.cpp 
    ThirdParty/poco/Foundation/src/PriorityNotificationQueue.cpp 
    ThirdParty/poco/Foundation/src/NullChannel.cpp 
    ThirdParty/poco/Foundation/src/NullStream.cpp 
    ThirdParty/poco/Foundation/src/NumberFormatter.cpp 
    ThirdParty/poco/Foundation/src/NumberParser.cpp 
    ThirdParty/poco/Foundation/src/Path.cpp 
    ThirdParty/poco/Foundation/src/PatternFormatter.cpp 
    ThirdParty/poco/Foundation/src/Pipe.cpp 
    ThirdParty/poco/Foundation/src/PipeImpl.cpp 
    ThirdParty/poco/Foundation/src/PipeStream.cpp 
    ThirdParty/poco/Foundation/src/Process.cpp 
    ThirdParty/poco/Foundation/src/PurgeStrategy.cpp 
    ThirdParty/poco/Foundation/src/RWLock.cpp 
    ThirdParty/poco/Foundation/src/Random.cpp 
    ThirdParty/poco/Foundation/src/RandomStream.cpp 
    ThirdParty/poco/Foundation/src/RefCountedObject.cpp 
    ThirdParty/poco/Foundation/src/RegularExpression.cpp 
    ThirdParty/poco/Foundation/src/RotateStrategy.cpp 
    ThirdParty/poco/Foundation/src/Runnable.cpp 
    ThirdParty/poco/Foundation/src/SHA1Engine.cpp 
    ThirdParty/poco/Foundation/src/Semaphore.cpp 
    ThirdParty/poco/Foundation/src/SharedLibrary.cpp 
    ThirdParty/poco/Foundation/src/SharedMemory.cpp 
    ThirdParty/poco/Foundation/src/SignalHandler.cpp 
    ThirdParty/poco/Foundation/src/SimpleFileChannel.cpp 
    ThirdParty/poco/Foundation/src/SortedDirectoryIterator.cpp 
    ThirdParty/poco/Foundation/src/SplitterChannel.cpp 
    ThirdParty/poco/Foundation/src/Stopwatch.cpp 
    ThirdParty/poco/Foundation/src/StreamChannel.cpp 
    ThirdParty/poco/Foundation/src/StreamConverter.cpp 
    ThirdParty/poco/Foundation/src/StreamCopier.cpp 
    ThirdParty/poco/Foundation/src/StreamTokenizer.cpp 
    ThirdParty/poco/Foundation/src/String.cpp 
    ThirdParty/poco/Foundation/src/NumericString.cpp 
    ThirdParty/poco/Foundation/src/StringTokenizer.cpp 
    ThirdParty/poco/Foundation/src/SynchronizedObject.cpp 
    ThirdParty/poco/Foundation/src/Task.cpp 
    ThirdParty/poco/Foundation/src/TaskManager.cpp 
    ThirdParty/poco/Foundation/src/TaskNotification.cpp 
    ThirdParty/poco/Foundation/src/TeeStream.cpp 
    ThirdParty/poco/Foundation/src/TemporaryFile.cpp 
    ThirdParty/poco/Foundation/src/TextConverter.cpp 
    ThirdParty/poco/Foundation/src/TextEncoding.cpp 
    ThirdParty/poco/Foundation/src/TextIterator.cpp 
    ThirdParty/poco/Foundation/src/TextBufferIterator.cpp 
    ThirdParty/poco/Foundation/src/Thread.cpp 
    ThirdParty/poco/Foundation/src/ThreadTarget.cpp 
    ThirdParty/poco/Foundation/src/ThreadLocal.cpp 
    ThirdParty/poco/Foundation/src/ThreadPool.cpp 
    ThirdParty/poco/Foundation/src/Timer.cpp 
    ThirdParty/poco/Foundation/src/Timespan.cpp 
    ThirdParty/poco/Foundation/src/Timestamp.cpp 
    ThirdParty/poco/Foundation/src/Timezone.cpp 
    ThirdParty/poco/Foundation/src/Token.cpp 
    ThirdParty/poco/Foundation/src/URI.cpp 
    ThirdParty/poco/Foundation/src/URIStreamFactory.cpp 
    ThirdParty/poco/Foundation/src/URIStreamOpener.cpp 
    ThirdParty/poco/Foundation/src/UTF16Encoding.cpp 
    ThirdParty/poco/Foundation/src/UTF32Encoding.cpp 
    ThirdParty/poco/Foundation/src/UTF8Encoding.cpp 
    ThirdParty/poco/Foundation/src/UTF8String.cpp 
    ThirdParty/poco/Foundation/src/UUID.cpp 
    ThirdParty/poco/Foundation/src/UUIDGenerator.cpp 
    ThirdParty/poco/Foundation/src/Unicode.cpp 
    ThirdParty/poco/Foundation/src/UnicodeConverter.cpp 
    ThirdParty/poco/Foundation/src/Var.cpp 
    ThirdParty/poco/Foundation/src/VarHolder.cpp 
    ThirdParty/poco/Foundation/src/VarIterator.cpp 
    ThirdParty/poco/Foundation/src/Void.cpp 
    ThirdParty/poco/Foundation/src/Windows1250Encoding.cpp 
    ThirdParty/poco/Foundation/src/Windows1251Encoding.cpp 
    ThirdParty/poco/Foundation/src/Windows1252Encoding.cpp 
    ThirdParty/poco/Foundation/src/adler32.c 
    ThirdParty/poco/Foundation/src/compress.c 
    ThirdParty/poco/Foundation/src/crc32.c 
    ThirdParty/poco/Foundation/src/deflate.c 
    ThirdParty/poco/Foundation/src/infback.c 
    ThirdParty/poco/Foundation/src/inffast.c 
    ThirdParty/poco/Foundation/src/inflate.c 
    ThirdParty/poco/Foundation/src/inftrees.c 
    ThirdParty/poco/Foundation/src/pcre_chartables.c 
    ThirdParty/poco/Foundation/src/pcre_compile.c 
    ThirdParty/poco/Foundation/src/pcre_exec.c 
    ThirdParty/poco/Foundation/src/pcre_fullinfo.c 
    ThirdParty/poco/Foundation/src/pcre_globals.c 
    ThirdParty/poco/Foundation/src/pcre_maketables.c 
    ThirdParty/poco/Foundation/src/pcre_newline.c 
    ThirdParty/poco/Foundation/src/pcre_ord2utf8.c 
    ThirdParty/poco/Foundation/src/pcre_study.c 
    ThirdParty/poco/Foundation/src/pcre_tables.c 
    ThirdParty/poco/Foundation/src/pcre_try_flipped.c 
    ThirdParty/poco/Foundation/src/pcre_ucd.c 
    ThirdParty/poco/Foundation/src/pcre_valid_utf8.c 
    ThirdParty/poco/Foundation/src/pcre_xclass.c 
    ThirdParty/poco/Foundation/src/trees.c 
    ThirdParty/poco/Foundation/src/zutil.c )

add_library( UWKCommon ${UWKCOMMON_SOURCE})

add_library( UWKPlugin MODULE ${UWK_PLUGIN_SOURCE} )

if (APPLE)

    set_target_properties( UWKPlugin PROPERTIES
        XCODE_ATTRIBUTE_INFOPLIST_FILE ${CMAKE_SOURCE_DIR}/Build/Mac/UWKPlugin/Info.plist
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Build/Mac/UWKPlugin/Info.plist 
        BUNDLE 1
        BUNDLE_EXTENSION bundle
        XCODE_ATTRIBUTE_WRAPPER_EXTENSION bundle
        XCODE_ATTRIBUTE_MACH_O_TYPE mh_bundle )

    target_link_libraries (UWKPlugin UWKCommon "-framework IOSurface" "-framework OpenGL")

else()
    target_link_libraries (UWKPlugin UWKCommon Iphlpapi)
endif()

