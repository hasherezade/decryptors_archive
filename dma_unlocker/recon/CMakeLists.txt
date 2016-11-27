cmake_minimum_required (VERSION 2.8)
project (DMA_recon)

set (srcs
    main.cpp
    PathCrawler.cpp
    FileSet.cpp
    ../filetypes/FileTypes.cpp
    ../decryptor/decryptor.cpp
    ../decryptor/aes256.cpp
    ../timeconv.cpp
    ../Params.cpp
)

set (hdrs
    PathCrawler.h
    FileSet.h
    ../filetypes/FileTypes.h
    ../decryptor/decryptor.h
    ../decryptor/aes256.h
    ../types.h
    ../timeconv.h
    ../Params.h
)

add_executable (DMA_recon ${hdrs} ${srcs} )
