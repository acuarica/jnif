
#ifndef JNIF_PARSER_CLASSFILEPARSER_HPP
#define JNIF_PARSER_CLASSFILEPARSER_HPP

#include "../base.hpp"
#include "../model/ClassFile.hpp"

namespace jnif::parser {

    class ClassFileParser : public model::ClassFile {
    public:

        explicit ClassFileParser(const u1* data, u4 len);

        static void parse(const u1* data, u4 len, ClassFile* classFile);

    };

}

#endif
