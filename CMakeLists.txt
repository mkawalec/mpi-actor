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

find_package(MPI REQUIRED)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH}
    ${MPI_CXX_LIBRARIES}
    )

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK ON)

find_package(Boost 1.52 REQUIRED COMPONENTS regex)

INCLUDE_DIRECTORIES(
    ${INCLUDE_DIRECTORIES}
    ${MPI_CXX_INCLUDE_PATH}
    ${Boost_INCLUDE_DIR}
    )

LINK_DIRECTORIES(
    ${MPI_CXX_LIBRARIES}
    ${Boost_LIBRARY_DIRS}
    )


set(actr_src
    src/actr/helpers.cpp
    src/actr/ActrBase.cpp
    )

set(actr_headers
    include/actr/exceptions.hpp
    include/actr/helpers.hpp
    include/actr/ActrBase.hpp
    include/actr.hpp
    )

add_library(
    actr
    SHARED
    ${actr_src}
    ${actr_headers}
    )

set(frog_headers
    include/frogs/Frog.hpp
    include/frogs/LandCell.hpp
    )

set(frogs_src
    src/frogs/Frog.cpp
    src/frogs/LandCell.cpp
    src/frogs.cpp
    ${frog_headers}
    )

add_executable(frogs ${frogs_src})
target_link_libraries(frogs -lm actr
    ${MPI_CXX_LIBRARIES}
    ${Boost_LIBRARIES}
    )
