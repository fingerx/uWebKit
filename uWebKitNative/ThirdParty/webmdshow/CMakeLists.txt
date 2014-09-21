

include_directories(common libvpx)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libvpx/x86/release)

#libmkvparser

file(GLOB LIBMKPARSER_SOURCE libmkvparser/*.cpp)

add_library(libmkparser STATIC ${LIBMKPARSER_SOURCE})

set_target_properties(libmkparser PROPERTIES COMPILE_DEFINITIONS "-DWIN32 -DNDEBUG -D_LIB -DNOMINMAX")

#VP8 Decoder

file(GLOB VP8DECODER_SOURCE vp8decoder/*.cpp vp8decoder/*.c vp8decoder/*.def vp8decoder/*.rc)

set (VP8DECODER_SOURCE ${VP8DECODER_SOURCE} common/cenumpins.cpp common/cfactory.cpp common/clockable.cpp
common/cmediasample.cpp common/cmediatypes.cpp common/cmemallocator.cpp common/comreg.cpp common/mediatypeutil.cpp
common/webmtypes.cpp)

add_library(vp8decoder SHARED ${VP8DECODER_SOURCE})

set_target_properties(vp8decoder PROPERTIES COMPILE_DEFINITIONS "-DWIN32 -DNDEBUG -D_WINDOWS -D_USRDLL -DVP8DECODER_EXPORTS")

target_link_libraries(vp8decoder strmiids.lib vpxmt)


#Splitter

#file(GLOB WEBMSPLIT_SOURCE webmsplit/*.cpp webmsplit/*.c webmsplit/*.def webmsplit/*.rc)

#add_library(webmsplit SHARED ${WEBMSPLIT_SOURCE})

#set_target_properties(webmsplit PROPERTIES COMPILE_DEFINITIONS "-DWIN32 -DNDEBUG -D_WINDOWS -D_USRDLL -D_WIN32_WINNT=0x0400 -DNOMINMAX")
