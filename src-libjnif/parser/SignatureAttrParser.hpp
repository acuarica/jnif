/*
 * SourceFileAttrParser.hpp
 *
 *  Created on: Jun 18, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_SIGNATUREATTRPARSER_HPP
#define JNIF_PARSER_SIGNATUREATTRPARSER_HPP

#include "BufferReader.hpp"
#include "../model/ClassFile.hpp"

#include "../model/attrs/SignatureAttr.hpp"

namespace jnif::parser {

    struct SignatureAttrParser {

        static constexpr const char* AttrName = "Signature";

        Attr* parse(BufferReader* br, ClassFile* cp, ConstPool::Index nameIndex) {
            ConstPool::Index sigIndex = br->readu2();
            Attr* attr = cp->_arena.create<SignatureAttr>(nameIndex, sigIndex, cp);
            return attr;
        }

    };

}

#endif
