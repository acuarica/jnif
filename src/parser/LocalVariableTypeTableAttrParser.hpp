/*
 * LocalVariableTypeTableAttrParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_LOCALVARIABLETYPETABLEATTRPARSER_HPP
#define JNIF_PARSER_LOCALVARIABLETYPETABLEATTRPARSER_HPP

namespace jnif {

class LocalVariableTypeTableAttrParser {
public:

	static constexpr const char* AttrName = "LocalVariableTypeTable";

	Attr* parse(BufferReader& br, ClassFile& constPool, ConstIndex nameIndex,
			void* args) {
		LabelManager& labelManager = *(LabelManager*) args;

		u2 count = br.readu2();

		LvtAttr* lvt = new LvtAttr(ATTR_LVTT, nameIndex, &constPool);

		for (u2 i = 0; i < count; i++) {
			LvtAttr::LvEntry e;

			u2 startPc = br.readu2();

			e.startPcLabel = labelManager.createLabel(startPc);

			e.startPc = startPc;
			e.len = br.readu2();
			e.varNameIndex = br.readu2();

			// Signature instead of descriptor.
			e.varDescIndex = br.readu2();
			e.index = br.readu2();

			lvt->lvt.push_back(e);
		}

		return lvt;
	}

};

}

#endif
