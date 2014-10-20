
include_directories(include)

set(ZLIB_HDR
    include/zconf.h
    include/zlib.h
    include/crc32.h
    include/deflate.h
    include/gzguts.h
    include/inffast.h
    include/inffixed.h
    include/inflate.h
    include/inftrees.h
    include/trees.h
    include/zutil.h
)

set(ZLIB_SRC
    source/adler32.c
    source/compress.c
    source/crc32.c
    source/deflate.c
    source/gzclose.c
    source/gzlib.c
    source/gzread.c
    source/gzwrite.c
    source/inflate.c
    source/infback.c
    source/inftrees.c
    source/inffast.c
    source/trees.c
    source/uncompr.c
    source/zutil.c
)

add_library(zlib STATIC ${ZLIB_SRC})