
#ifndef JNIF_PARSER_CLASSFILEPARSER_HPP
#define JNIF_PARSER_CLASSFILEPARSER_HPP

#include "../base.hpp"

namespace jnif::parser {

    class ClassFileParser {
    public:

        static void parse(const u1* data, int len, class ClassFile* cf);

    };
}

#endif
