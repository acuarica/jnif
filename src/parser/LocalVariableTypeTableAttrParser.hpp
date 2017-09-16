/*
 * LocalVariableTypeTableAttrParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_LOCALVARIABLETYPETABLEATTRPARSER_HPP
#define JNIF_PARSER_LOCALVARIABLETYPETABLEATTRPARSER_HPP

namespace jnif::parser {

    class LocalVariableTypeTableAttrParser {
    public:

        static constexpr const char* AttrName = "LocalVariableTypeTable";

        Attr* parse(BufferReader* br, ClassFile* cp, ConstPool::Index nameIndex,
                    LabelManager* labelManager) {

            u2 count = br->readu2();

            LvtAttr* lvt = cp->_arena.create<LvtAttr>(ATTR_LVTT, nameIndex, cp);

            for (u2 i = 0; i < count; i++) {
                LvtAttr::LvEntry e;

                u2 startPc = br->readu2();

                e.startPcLabel = labelManager->createLabel(startPc);

                e.startPc = startPc;
                e.len = br->readu2();
                e.varNameIndex = br->readu2();

                // Signature instead of descriptor.
                e.varDescIndex = br->readu2();
                e.index = br->readu2();

                lvt->lvt.push_back(e);
            }

            return lvt;
        }

    };

}

#endif
