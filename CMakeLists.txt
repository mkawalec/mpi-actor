cmake_minimum_required(VERSION 2.8)
project(actor)
include_directories(src include)

if (CMAKE_COMPILER_IS_GNUCXX)
    message(status "Setting GCC flags!")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -ffast-math -ftree-vectorize -O3 -Wall -Wextra -Wno-ignored-qualifiers -pedantic -W -std=c++0x")
else()
    message(status "gcc not found!")
    return()
endif()


set(actr_src
    src/actr/helpers.cpp
    )

set(actr_headers
    include/actr/helpers.hpp
    )

add_library(
    actr
    SHARED
    ${actr_src}
    ${actr_headers}
    )
