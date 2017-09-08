
#include "../ClassFile.hpp"

#include "ClassFileParser.hpp"

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

    void ClassFileParser::parse(const u1* data, int len, ClassFile* cf) {
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
        parser.parse(&br, cf);
    }

}
