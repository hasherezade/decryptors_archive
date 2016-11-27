cmake_minimum_required (VERSION 2.8)
project (DMA_unlocker)

set (recon_srcs
    ../recon/PathCrawler.cpp
    ../recon/FileSet.cpp
)

set (decryptor_srcs
    ../decryptor/decryptor.cpp
    ../decryptor/aes256.cpp
)

set (srcs
    main.cpp
    Unlocker.cpp
    ../keygen/keygen.cpp
    ../keygen/DmaKeygen.cpp
    ../filetypes/filetypes.cpp
    ../filetypes/TypeValidator.cpp
    ../cracking_crawler/CrackedSet.cpp
    ../dma_crack.cpp
    ../timeconv.cpp
    ../Params.cpp
)

set (recon_hdrs
    ../recon/PathCrawler.h
    ../recon/FileSet.h
)

set (decryptor_hdrs
    ../decryptor/decryptor.h
    ../decryptor/aes256.h
)
    
set (hdrs
    Unlocker.h
    ../keygen/keygen.h
    ../keygen/DmaKeygen.h
    ../filetypes/filetypes.h
    ../filetypes/TypeValidator.h
    ../cracking_crawler/CrackedSet.h
    ../dma_crack.h
    ../util.h
    ../types.h
    ../timeconv.h
    ../Params.h
)

SOURCE_GROUP("Header Files\\recon" FILES ${recon_hdrs} )
SOURCE_GROUP("Source Files\\recon" FILES ${recon_srcs} )

SOURCE_GROUP("Header Files\\decryptor" FILES ${decryptor_hdrs} )
SOURCE_GROUP("Source Files\\decryptor" FILES ${decryptor_srcs} )

add_executable (DMA_unlocker ${hdrs} ${srcs} ${recon_hdrs} ${recon_srcs} ${decryptor_hdrs} ${decryptor_srcs})
