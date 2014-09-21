

# webmdshow and libwebm commit info for what this is based on
# https://github.com/webmproject/webmdshow/commit/79985163b29cebbe37987a26d3802346b6ae51a9
# https://github.com/webmproject/libwebm/commit/1156da8d29f926b6b0063974eb8a75a82ab7f9b2

include_directories(common libmkvparser libvpx libogg libvorbis)

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libvpx/x86/release 
				 	 ${CMAKE_CURRENT_SOURCE_DIR}/libogg/x86/release 
				     ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/x86/release)
 else()
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libvpx/x64/release 
				 	 ${CMAKE_CURRENT_SOURCE_DIR}/libogg/x64/release 
				     ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/x64/release)
 endif()

#libmkvparser

file(GLOB LIBMKPARSER_SOURCE libmkvparser/*.cpp)

add_library(libmkparser STATIC ${LIBMKPARSER_SOURCE})

set_target_properties(libmkparser PROPERTIES COMPILE_DEFINITIONS "WIN32;NDEBUG;_LIB;NOMINMAX")

#VP8 Decoder

file(GLOB VP8DECODER_SOURCE vp8decoder/*.cpp vp8decoder/*.c vp8decoder/*.def vp8decoder/*.rc)

set (VP8DECODER_SOURCE ${VP8DECODER_SOURCE} common/cenumpins.cpp common/cfactory.cpp common/clockable.cpp
common/cmediasample.cpp common/cmediatypes.cpp common/cmemallocator.cpp common/comreg.cpp common/mediatypeutil.cpp
common/webmtypes.cpp)

add_library(vp8decoder SHARED ${VP8DECODER_SOURCE})

set_target_properties(vp8decoder PROPERTIES COMPILE_DEFINITIONS "WIN32;NDEBUG;_WINDOWS;_USRDLL;VP8DECODER_EXPORTS")

target_link_libraries(vp8decoder strmiids vpxmt)

# Vorbis Decoder

file(GLOB VORBISDECODER_SOURCE webmvorbisdecoder/*.cpp webmvorbisdecoder/*.c webmvorbisdecoder/*.def webmvorbisdecoder/*.rc)

set (VORBISDECODER_SOURCE ${VORBISDECODER_SOURCE} common/cenumpins.cpp common/cfactory.cpp common/clockable.cpp
common/cmediasample.cpp common/cmediatypes.cpp common/cmemallocator.cpp common/comreg.cpp common/mediatypeutil.cpp
common/vorbistypes.cpp common/webmtypes.cpp)

add_library(webmvorbisdecoder SHARED ${VORBISDECODER_SOURCE})

set_target_properties(webmvorbisdecoder PROPERTIES COMPILE_DEFINITIONS "WIN32;_WINDOWS;_USRDLL")

target_link_libraries(webmvorbisdecoder strmiids libogg_static libvorbis_static)


#Splitter

file(GLOB WEBMSPLIT_SOURCE webmsplit/*.cpp webmsplit/*.c webmsplit/*.def webmsplit/*.rc)

set(WEBMSPLIT_SOURCE ${WEBMSPLIT_SOURCE} common/cenumpins.cpp common/cfactory.cpp common/clockable.cpp common/cmediasample.cpp
										 common/cmediatypes.cpp common/cmemallocator.cpp common/comreg.cpp common/graphutil.cpp
										 common/mediatypeutil.cpp common/vorbistypes.cpp common/webmtypes.cpp)

add_library(webmsplit SHARED ${WEBMSPLIT_SOURCE})

set_target_properties(webmsplit PROPERTIES COMPILE_DEFINITIONS "WIN32;NDEBUG;_WINDOWS;_USRDLL;_WIN32_WINNT=0x0400;NOMINMAX")

target_link_libraries(webmsplit strmiids libmkparser)

