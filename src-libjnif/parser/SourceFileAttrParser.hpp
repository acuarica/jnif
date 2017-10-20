/*
 * SourceFileAttrParser.hpp
 *
 *  Created on: Jun 18, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_SOURCEFILEATTRPARSER_HPP
#define JNIF_PARSER_SOURCEFILEATTRPARSER_HPP

#include <jnif.hpp>

namespace jnif {

    namespace parser {

        struct SourceFileAttrParser {

            static constexpr const char* AttrName = "SourceFile";

            Attr* parse(BufferReader* br, ClassFile* cp, ConstPool::Index nameIndex) {
                u2 sourceFileIndex = br->readu2();
                Attr* attr = cp->_arena.create<SourceFileAttr>(nameIndex, sourceFileIndex, cp);
                return attr;
            }

        };

    }
}

#endif
