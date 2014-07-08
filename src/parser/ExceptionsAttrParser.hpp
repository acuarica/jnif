/*
 * ExceptionsAttrParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_EXCEPTIONSATTRPARSER_HPP
#define JNIF_PARSER_EXCEPTIONSATTRPARSER_HPP

namespace jnif {

/**
 * The ExceptionsAttrParser parses the Exceptions attribute of a method.
 */
class ExceptionsAttrParser {
public:

	static constexpr const char* AttrName = "Exceptions";

	Attr* parse(BufferReader& br, ClassFile& constPool, ConstIndex nameIndex,
			void*) {
		u2 len = br.readu2();

		std::vector<ConstIndex> es;
		for (int i = 0; i < len; i++) {
			ConstIndex exceptionIndex = br.readu2();

			es.push_back(exceptionIndex);
		}

		Attr* attr = new ExceptionsAttr(nameIndex, &constPool, es);

		return attr;
	}

};

}

#endif
