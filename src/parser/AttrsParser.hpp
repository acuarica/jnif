/*
 * AttrsParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_ATTRSPARSER_HPP
#define JNIF_PARSER_ATTRSPARSER_HPP

#include "BufferReader.hpp"
#include "../ClassFile.hpp"
#include "../Attr.hpp"

namespace jnif::parser {

    template<class... TAttrParsers>
    struct AttrParser {
        template<class... TArgs>
        Attr* parse(
            u2 nameIndex, u4 len, const u1* data, const String&,
            ClassFile* cp, TArgs...
            ) {
            return cp->_arena.create<UnknownAttr>(nameIndex, len, data, cp);
        }
    };

    template<class TAttrParser, class... TAttrParsers>
    struct AttrParser<TAttrParser, TAttrParsers...> : AttrParser<TAttrParsers...> {

        template<class... TArgs>
        Attr* parse(u2 nameIndex, u4 len, const u1* data, const String& attrName,
                    ClassFile* cp, TArgs... args) {
            if (attrName == TAttrParser::AttrName) {
                BufferReader br(data, len);
                return TAttrParser().parse(&br, cp, nameIndex, args...);
            } else {
                return AttrParser<TAttrParsers...>::parse(
                    nameIndex, len, data, attrName, cp, args...);
            }
        }

    };

    template<class... TAttrParsers>
    struct AttrsParser {

        template<class... TArgs>
        void parse(BufferReader* br, ClassFile* cp, Attrs* as, TArgs... args ) {
            u2 attrCount = br->readu2();

            for (int i = 0; i < attrCount; i++) {
                u2 nameIndex = br->readu2();
                u4 len = br->readu4();
                const u1* data = br->pos();

                br->skip(len);

                String attrName = cp->getUtf8(nameIndex);

                Attr* a = AttrParser<TAttrParsers...>().parse(
                    nameIndex, len, data, attrName, cp, args...);
                as->add(a);
            }
        }

    };


}

#endif
