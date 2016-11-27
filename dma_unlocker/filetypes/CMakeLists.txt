cmake_minimum_required (VERSION 2.8)
project (DMA_filetypes)

set (srcs
    main.cpp
    FileTypes.cpp
)

set (hdrs
    FileTypes.h
    ../types.h
)

add_executable (DMA_filetypes ${hdrs} ${srcs} )
