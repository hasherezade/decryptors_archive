cmake_minimum_required (VERSION 2.8)
project (PrincessKeygen)

add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

set (srcs
    aes_crypt.cpp
    main.cpp
)

set (hdrs
    aes_crypt.h
)

add_executable (PrincessKeygen ${hdrs} ${srcs})