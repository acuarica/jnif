cmake_minimum_required(VERSION 3.5)
project(jnif)

set(CMAKE_CXX_STANDARD 11)

find_package(JNI)
find_package(Java QUIET)
include(UseJava)

add_library(jnif
        src-libjnif/parser.cpp
        src-libjnif/base.cpp
        src-libjnif/jnif.hpp
        src-libjnif/printer.cpp
        src-libjnif/writer.cpp
        src-libjnif/stream.cpp
        src-libjnif/jar.cpp
        src-libjnif/model.cpp
        src-libjnif/analysis.cpp
        src-libjnif/zip/ioapi.c
        src-libjnif/zip/ioapi.h
        src-libjnif/zip/unzip.c
        src-libjnif/zip/unzip.h)

add_executable(jnifp
        src-jnifp/jnifp.cpp)

add_executable(testunit
        src-testunit/testunit.cpp)

add_executable(testjars
        src-testjars/testjars.cpp
        src-testjars/tests.cpp
        src-testjars/tests.hpp)

add_library(testagent
        src-testagent/testagent.cpp
        src-testagent/frtlog.hpp
        src-testagent/frexception.hpp
        src-testagent/frinstr.hpp
        src-testagent/frinstrclass.cpp
        src-testagent/frinstrclient.cpp
        src-testagent/frinstrhandler.cpp
        src-testagent/frjvmti.hpp
        src-testagent/frlog.hpp
        src-testagent/frstamp.cpp
        src-testagent/frstamp.hpp
        src-testagent/frthread.cpp
        src-testagent/frthread.hpp)

target_include_directories(jnif INTERFACE src-libjnif)
target_include_directories(testagent PRIVATE ${JNI_INCLUDE_DIRS})

target_link_libraries(jnif z)
target_link_libraries(jnifp jnif)
target_link_libraries(testunit jnif)
target_link_libraries(testjars jnif)
target_link_libraries(testagent jnif)
