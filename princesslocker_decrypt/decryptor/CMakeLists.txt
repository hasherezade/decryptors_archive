cmake_minimum_required (VERSION 2.8)
project (PrincessDecryptor)

add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

set (srcs
	decryptor.cpp
	main.cpp
	PathCrawler.cpp
)

set (hdrs
	decryptor.h
	PathCrawler.h
)
add_executable (PrincessDecryptor ${rsrc} ${hdrs} ${srcs})