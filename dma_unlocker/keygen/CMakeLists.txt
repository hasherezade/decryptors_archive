cmake_minimum_required (VERSION 2.8)
project (DMA_keygen)

set (srcs
    main.cpp
    DmaKeygen.cpp
    keygen.cpp
)

set (hdrs
    DmaKeygen.h
    keygen.h
    ../types.h
)

add_executable (DMA_keygen ${hdrs} ${srcs} )
