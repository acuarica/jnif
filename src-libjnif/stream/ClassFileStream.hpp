
#ifndef ASDF
#define ASDF

#include "../model/ClassFile.hpp"

namespace jnif::stream {

    class ClassFileStream : public model::ClassFile {
    public:

        explicit ClassFileStream(const char* fileName);

    };
}
#endif
