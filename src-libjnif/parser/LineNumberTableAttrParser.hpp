/*
 * LineNumberTableAttrParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_LINENUMBERTABLEATTRPARSER_HPP
#define JNIF_PARSER_LINENUMBERTABLEATTRPARSER_HPP

#include "LabelManager.hpp"

namespace jnif::parser {

    class LineNumberTableAttrParser {
    public:

        static constexpr const char* AttrName = "LineNumberTable";

        Attr* parse(BufferReader* br, ClassFile* cp, ConstPool::Index nameIndex,
                    LabelManager* labelManager) {

            u2 lntlen = br->readu2();

            LntAttr* lnt = cp->_arena.create<LntAttr>(nameIndex, cp);

            for (int i = 0; i < lntlen; i++) {
                LntAttr::LnEntry e;
                u2 startpc = br->readu2();
                u2 lineno = br->readu2();

                e.startPcLabel = labelManager->createLabel(startpc);

                e.startpc = startpc;
                e.lineno = lineno;

                lnt->lnt.push_back(e);
            }

            return lnt;
        }

    };

}

#endif
