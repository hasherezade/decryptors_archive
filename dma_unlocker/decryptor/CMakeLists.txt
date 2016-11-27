cmake_minimum_required (VERSION 2.8)
project (DMA_decryptor)

set (srcs
    main.cpp
    decryptor.cpp
    aes256.cpp
)

set (hdrs
    decryptor.h
    aes256.h
    ../types.h
)

add_executable (DMA_decryptor ${hdrs} ${srcs} )
