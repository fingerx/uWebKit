

include_directories(common libvpx)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libvpx/x86/release)

add_definitions( -DWIN32 -DNDEBUG -D_WINDOWS -D_USRDLL -DVP8DECODER_EXPORTS)

file(GLOB VP8DECODER_SOURCE vp8decoder/*.cpp vp8decoder/*.c)

set (VP8DECODER_SOURCE ${VP8DECODER_SOURCE} common/cenumpins.cpp common/cfactory.cpp common/clockable.cpp
common/cmediasample.cpp common/cmediatypes.cpp common/cmemallocator.cpp common/comreg.cpp common/mediatypeutil.cpp
common/webmtypes.cpp)

add_library(vp8decoder SHARED ${VP8DECODER_SOURCE})



target_link_libraries(vp8decoder strmiids.lib vpxmt)
