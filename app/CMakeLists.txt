cmake_minimum_required(VERSION 3.4.1)

add_library(
        eventprocessor
        SHARED
        src/main/cpp/eventprocessor_wrap.cpp
        #src/main/cpp/eventprocessor.cpp
        #src/main/cpp/eventprocessor.hpp
)

#add_library(
#        plasma
#        SHARED
#        src/main/cpp/plasma.c)

find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

target_link_libraries( # Specifies the target library.
        eventprocessor
        #plasma
        android
        jnigraphics
        log
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})