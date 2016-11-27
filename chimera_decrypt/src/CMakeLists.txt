cmake_minimum_required (VERSION 2.8)
project (chimera_dec)

set (srcs
    main.cpp
)

set (hdrs
    wrapper.h
)

add_executable (chimera_dec ${hdrs} ${srcs})
