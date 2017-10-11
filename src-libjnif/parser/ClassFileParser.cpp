
#include <jnif.hpp>

#include "BufferReader.hpp"
#include "ConstPoolParser.hpp"
#include "ClassParser.hpp"
#include "CodeAttrParser.hpp"
#include "ExceptionsAttrParser.hpp"
#include "LineNumberTableAttrParser.hpp"
#include "LocalVariableTableAttrParser.hpp"
#include "LocalVariableTypeTableAttrParser.hpp"
#include "StackMapTableAttrParser.hpp"
#include "SourceFileAttrParser.hpp"
#include "SignatureAttrParser.hpp"

namespace jnif::parser {

    ClassFileParser::ClassFileParser(const u1* data, u4 len) {
        parse(data, len, this);
    }

    void ClassFileParser::parse(const u1* data, u4 len, ClassFile* classFile) {
        BufferReader br(data, len);
        ClassParser<
                ConstPoolParser,
                AttrsParser<
                        SourceFileAttrParser,
                        SignatureAttrParser>,
                AttrsParser<
                        CodeAttrParser<
                                LineNumberTableAttrParser,
                                LocalVariableTableAttrParser,
                                LocalVariableTypeTableAttrParser,
                                StackMapTableAttrParser>,
                        ExceptionsAttrParser,
                        SignatureAttrParser>,
                AttrsParser<
                        SignatureAttrParser>
        > parser;
        parser.parse(&br, classFile);
    }

}
