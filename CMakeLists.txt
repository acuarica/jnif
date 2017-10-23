cmake_minimum_required(VERSION 3.5)
project(jnif)

set(CMAKE_CXX_STANDARD 11)

include_directories(src-libjnif)

add_library(jnif
        src-libjnif/parser.cpp
        src-libjnif/Dominator.hpp
        src-libjnif/base.cpp
        src-libjnif/jnif.hpp
        src-libjnif/printer.cpp
        src-libjnif/writer.cpp
        src-libjnif/stream.cpp
        src-libjnif/jar.cpp
        src-libjnif/model.cpp
        src-libjnif/analysis.cpp
        src-libjnif/zip/ioapi.cpp
        src-libjnif/zip/ioapi.h src-libjnif/zip/unzip.cpp src-libjnif/zip/unzip.h)

add_executable(jnifp src-jnifp/jnifp.cpp)
add_executable(testunit src-testunit/testunit.cpp)
add_executable(testjars src-testjars/testjars.cpp src-testjars/tests.cpp src-testjars/tests.hpp)

target_link_libraries(jnif z)
target_link_libraries(jnifp jnif)
target_link_libraries(testunit jnif)
target_link_libraries(testjars jnif)
