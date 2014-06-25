/*
 * LineNumberTableAttrParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_LINENUMBERTABLEATTRPARSER_HPP
#define JNIF_PARSER_LINENUMBERTABLEATTRPARSER_HPP

#include "LabelManager.hpp"

namespace jnif {

class LineNumberTableAttrParser {
public:

	static constexpr const char* AttrName = "LineNumberTable";

	Attr* parse(BufferReader& br, ClassFile& constPool, ConstIndex nameIndex,
			void* args) {
		LabelManager& labelManager = *(LabelManager*) args;

		u2 lntlen = br.readu2();

		LntAttr* lnt = new LntAttr(nameIndex, &constPool);

		for (int i = 0; i < lntlen; i++) {
			LntAttr::LnEntry e;
			u2 startpc = br.readu2();
			u2 lineno = br.readu2();

			e.startPcLabel = labelManager.createLabel(startpc);

			e.startpc = startpc;
			e.lineno = lineno;

			lnt->lnt.push_back(e);
		}

		return lnt;
	}

};

}

#endif
