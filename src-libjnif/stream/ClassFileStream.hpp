
#ifndef JNIF_STREAM_CLASSFILESTREAM_HPP
#define JNIF_STREAM_CLASSFILESTREAM_HPP

#include "../model/ClassFile.hpp"

namespace jnif::stream {

    /**
     * Represents a ClassFile created from a disk file.
     */
    class ClassFileStream : public model::ClassFile {
    public:

        /**
         * Constructs a ClassFile parsing the specified fileName from disk.
         *
         * @param fileName The name of the file to parse.
         */
        explicit ClassFileStream(const char* fileName);

    };
}

#endif
