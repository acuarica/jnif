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
    struct ExceptionsAttrParser {

        static constexpr const char* AttrName = "Exceptions";

        Attr* parse(BufferReader* br, ClassFile* cp, ConstPool::Index nameIndex) {
            u2 len = br->readu2();

            std::vector<ConstPool::Index> es;
            for (int i = 0; i < len; i++) {
                ConstPool::Index exceptionIndex = br->readu2();

                es.push_back(exceptionIndex);
            }

            Attr* attr = cp->_arena.create<ExceptionsAttr>(nameIndex, cp, es);

            return attr;
        }

    };

}

#endif
