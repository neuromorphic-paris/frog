cmake_minimum_required(VERSION 3.4.1)

add_library(
        eventprocessor
        SHARED
        src/main/cpp/eventprocessor_wrap.cpp
        src/main/cpp/eventprocessor.cpp
        src/main/cpp/eventprocessor.hpp
        src/main/cpp/eventprocessor1L.cpp
)

find_library(
        log-lib
        log)

target_link_libraries(
        eventprocessor
        android
        jnigraphics
        log
        ${log-lib})