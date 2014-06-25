/*
 * LocalVariableTableAttrParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_LOCALVARIABLETABLEATTRPARSER_HPP
#define JNIF_PARSER_LOCALVARIABLETABLEATTRPARSER_HPP

namespace jnif {

class LocalVariableTableAttrParser {
public:

	static constexpr const char* AttrName = "LocalVariableTable";

	Attr* parse(BufferReader& br, ClassFile& constPool, ConstIndex nameIndex,
			void* args) {
		LabelManager& labelManager = *(LabelManager*) args;

		u2 count = br.readu2();

		LvtAttr* lvt = new LvtAttr(ATTR_LVT, nameIndex, &constPool);

		for (u2 i = 0; i < count; i++) {
			LvtAttr::LvEntry e;

			u2 startPc = br.readu2();

			e.startPcLabel = labelManager.createLabel(startPc);

			e.startPc = startPc;
			e.len = br.readu2();
			//u2 len = e.len;
			//u2 endPc = startPc + len;
			//labelManager
			//Error::check(endPc <= labelManager.codeLen, "inv endPc");
			//Error::check(endPc <= labelManager.codeLen, "inv endPc");

			//e.endPcLabel = labelManager.createLabel(endPc);
			//Error::assert(e.endPcLabel != NULL, "asdf");
			//Error::assert(e.endPcLabel->label()->id >= 1, "1");
			//Error::assert(e.endPcLabel->label()->id <= 65536, "65536");

			e.varNameIndex = br.readu2();
			e.varDescIndex = br.readu2();
			e.index = br.readu2();

			lvt->lvt.push_back(e);
		}

		return lvt;
	}

};

}

#endif
