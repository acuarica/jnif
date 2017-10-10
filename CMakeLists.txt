cmake_minimum_required(VERSION 3.8)
project(jnif)

set(CMAKE_CXX_STANDARD 17)

include_directories( src-libjnif )

set(SOURCE_FILES
        src-libjnif/analysis/ComputeFrames.hpp
        src-libjnif/analysis/FrameGenerator.hpp
        src-libjnif/analysis/SmtBuilder.hpp
        src-libjnif/jar/ioapi.cpp
        src-libjnif/jar/ioapi.h
        src-libjnif/jar/JarFile.hpp
        src-libjnif/jar/unzip.cpp
        src-libjnif/jar/unzip.h
        src-libjnif/model/attrs/Attr.cpp
        src-libjnif/model/attrs/Attr.hpp
        src-libjnif/model/attrs/SourceFileAttr.hpp
        src-libjnif/model/Arena.cpp
        src-libjnif/model/Arena.hpp
        src-libjnif/model/ClassFile.cpp
        src-libjnif/model/ClassFile.hpp
        src-libjnif/model/ConstPool.cpp
        src-libjnif/model/ConstPool.hpp
        src-libjnif/model/Inst.cpp
        src-libjnif/model/Inst.hpp
        src-libjnif/model/InstList.cpp
        src-libjnif/model/InstList.hpp
        src-libjnif/model/Opcode.hpp
        src-libjnif/model/Type.cpp
        src-libjnif/model/Type.hpp
        src-libjnif/model/TypeFactory.cpp
        src-libjnif/model/TypeFactory.hpp
        src-libjnif/model/Version.cpp
        src-libjnif/model/Version.hpp
        src-libjnif/parser/AttrsParser.hpp
        src-libjnif/parser/BootstrapMethodsAttrParser.hpp
        src-libjnif/parser/BufferReader.hpp
        src-libjnif/parser/ClassFileParser.cpp
        src-libjnif/parser/ClassFileParser.hpp
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
        src-libjnif/stream/ClassFileStream.hpp
        src-libjnif/base.hpp
        src-libjnif/BasicBlock.cpp
        src-libjnif/BasicBlock.hpp
        src-libjnif/ClassHierarchy.cpp
        src-libjnif/ClassHierarchy.hpp
        src-libjnif/ControlFlowGraph.cpp
        src-libjnif/ControlFlowGraph.hpp
        src-libjnif/Dominator.hpp
        src-libjnif/Error.cpp
        src-libjnif/Error.hpp
        src-libjnif/Frame.cpp
        src-libjnif/Frame.hpp
        src-libjnif/jnif.hpp
        src-libjnif/printer.cpp
        src-libjnif/writer.cpp
        src-libjnif/model/attrs/SignatureAttr.hpp src-libjnif/model/IClassPath.hpp src-libjnif/stream/ClassFileStream.cpp src-libjnif/model/Member.cpp)

add_library(jnif ${SOURCE_FILES})
add_executable(jnifp src-jnifp/jnifp.cpp)
add_executable(testunit src-testunit/testunit.cpp)
add_executable(testjars src-testjars/testjars.cpp src-testjars/tests.cpp src-testjars/tests.hpp)

target_link_libraries (jnif z)
target_link_libraries (jnifp jnif)
target_link_libraries (testunit jnif)
target_link_libraries (testjars jnif)
