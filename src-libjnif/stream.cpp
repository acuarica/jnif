//
// Created by Luigi on 07.10.17.
//

#include "jnif.hpp"
#include <fstream>
#include <iostream>

namespace jnif {

    namespace stream {

        ClassFileStream::ClassFileStream(const char* fileName) {
            std::ifstream ifs(fileName, std::ios::in | std::ios::binary | std::ios::ate);

            if (!ifs.is_open()) {
                int m;
                ifs >> m;
                throw "File not opened!";
            }

            u4 fileSize = ifs.tellg();
            u1* buffer = new u1[fileSize];

            ifs.seekg(0, std::ios::beg);
            if (!ifs.read((char*) buffer, fileSize)) {
                throw "File not opened!";
            }

            parser::ClassFileParser::parse(buffer, fileSize, this);

            delete[] buffer;
        }

    }
}
