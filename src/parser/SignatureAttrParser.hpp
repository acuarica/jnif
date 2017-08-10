/*
 * SourceFileAttrParser.hpp
 *
 *  Created on: Jun 18, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_SIGNATUREATTRPARSER_HPP
#define JNIF_PARSER_SIGNATUREATTRPARSER_HPP

namespace jnif {

class SignatureAttrParser {
public:

	static constexpr const char* AttrName = "Signature";

	Attr* parse(BufferReader& br, ClassFile& cp, ConstIndex nameIndex, void*) {
		u2 signatureIndex = br.readu2();
		Attr* attr = cp._arena.create<SignatureAttr>(nameIndex, signatureIndex, &cp);
		return attr;
	}

};

}

#endif
