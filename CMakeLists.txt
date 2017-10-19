cmake_minimum_required(VERSION 3.5)
project(jnif)

set(CMAKE_CXX_STANDARD 11)

include_directories( src-libjnif )

set(SOURCE_FILES
        src-libjnif/analysis/ComputeFrames.hpp
        src-libjnif/analysis/FrameGenerator.hpp
        src-libjnif/analysis/SmtBuilder.hpp
        src-libjnif/jar/ioapi.cpp
        src-libjnif/jar/ioapi.h
        src-libjnif/jar/unzip.cpp
        src-libjnif/jar/unzip.h
        src-libjnif/Arena.cpp
        src-libjnif/parser/BootstrapMethodsAttrParser.hpp
        src-libjnif/parser/BufferReader.hpp
        src-libjnif/parser/ClassFileParser.cpp
        src-libjnif/parser/ClassParser.hpp
        src-libjnif/parser/CodeAttrParser.hpp
        src-libjnif/parser/ConstPoolParser.hpp
        src-libjnif/parser/ExceptionsAttrParser.hpp
        src-libjnif/parser/LabelManager.hpp
        src-libjnif/parser/LineNumberTableAttrParser.hpp
        src-libjnif/parser/LocalVariableTableAttrParser.hpp
        src-libjnif/parser/LocalVariableTypeTableAttrParser.hpp
        src-libjnif/parser/SignatureAttrParser.hpp
        src-libjnif/parser/SourceFileAttrParser.hpp
        src-libjnif/parser/StackMapTableAttrParser.hpp
        src-libjnif/BasicBlock.cpp
        src-libjnif/ControlFlowGraph.cpp
        src-libjnif/Dominator.hpp
        src-libjnif/Error.cpp
        src-libjnif/Frame.cpp
        src-libjnif/jnif.hpp
        src-libjnif/printer.cpp
        src-libjnif/writer.cpp
        src-libjnif/stream/ClassFileStream.cpp
        src-libjnif/jar/jar.cpp src-libjnif/model.cpp)

add_library(jnif ${SOURCE_FILES})
add_executable(jnifp src-jnifp/jnifp.cpp)
add_executable(testunit src-testunit/testunit.cpp)
add_executable(testjars src-testjars/testjars.cpp src-testjars/tests.cpp src-testjars/tests.hpp)

target_link_libraries (jnif z)
target_link_libraries (jnifp jnif)
target_link_libraries (testunit jnif)
target_link_libraries (testjars jnif)
