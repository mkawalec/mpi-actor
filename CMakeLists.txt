cmake_minimum_required(VERSION 2.8)
project(actor)
include_directories(src include)

set(actr_src
    )

set(actr_headers
    )

add_library(
    actr
    DYNAMIC
    ${actr_src}
    ${actr_headers}
    )
