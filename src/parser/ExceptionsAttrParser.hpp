/*
 * ExceptionsAttrParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_EXCEPTIONSATTRPARSER_HPP
#define JNIF_PARSER_EXCEPTIONSATTRPARSER_HPP

namespace jnif::parser {

/**
 * The ExceptionsAttrParser parses the Exceptions attribute of a method.
 */
    class ExceptionsAttrParser {
    public:

        static constexpr const char* AttrName = "Exceptions";

        Attr* parse(BufferReader* br, ClassFile* cp, ConstIndex nameIndex) {
            u2 len = br->readu2();

            std::vector<ConstIndex> es;
            for (int i = 0; i < len; i++) {
                ConstIndex exceptionIndex = br->readu2();

                es.push_back(exceptionIndex);
            }

            Attr* attr = cp->_arena.create<ExceptionsAttr>(nameIndex, cp, es);

            return attr;
        }

    };

}

#endif
