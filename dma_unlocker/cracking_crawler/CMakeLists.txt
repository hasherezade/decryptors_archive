cmake_minimum_required (VERSION 2.8)
project (DMA_crawler)

set (recon_srcs
    ../recon/PathCrawler.cpp
    ../recon/FileSet.cpp
)

set (srcs
    main.cpp
    CrackedSet.cpp
    ../decryptor/decryptor.cpp
    ../decryptor/aes256.cpp
    ../keygen/keygen.cpp
    ../filetypes/filetypes.cpp
    ../filetypes/TypeValidator.cpp
    ../dma_crack.cpp
    ../timeconv.cpp
    ../Params.cpp
)

set (recon_hdrs
    ../recon/PathCrawler.h
    ../recon/FileSet.h
)
    
set (hdrs
    CrackedSet.h
    ../decryptor/decryptor.h
    ../decryptor/aes256.h
    ../keygen/keygen.h
    ../filetypes/filetypes.h
    ../filetypes/TypeValidator.h
    ../dma_crack.h
    ../types.h
    ../timeconv.h
    ../Params.h
)

SOURCE_GROUP("Header Files\\recon" FILES ${recon_hdrs} )
SOURCE_GROUP("Source Files\\recon" FILES ${recon_srcs} )

add_executable (DMA_crawler ${hdrs} ${srcs} ${recon_hdrs} ${recon_srcs})
