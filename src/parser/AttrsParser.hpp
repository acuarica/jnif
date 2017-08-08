/*
 * AttrsParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_ATTRSPARSER_HPP
#define JNIF_PARSER_ATTRSPARSER_HPP

namespace jnif {

template<typename ... TAttrParserList>
class AttrsParser {
public:

	void parse(BufferReader& br, ClassFile& cp, Attrs& as, void* args = NULL) {
		u2 attrCount = br.readu2();

		for (int i = 0; i < attrCount; i++) {
			u2 nameIndex = br.readu2();
			u4 len = br.readu4();
			const u1* data = br.pos();

			br.skip(len);

			String attrName = cp.getUtf8(nameIndex);

			Attr* a = _parse<void, TAttrParserList...>(nameIndex, len, data, attrName, cp, args);
			as.add(a);
		}
	}

private:

	template<typename __TVoid>
	Attr* _parse(u2 nameIndex, u4 len, const u1* data, const String&,
			ClassFile& cp, void*) {
    return cp._arena.create<UnknownAttr>(nameIndex, len, data, &cp);
	}

	template<typename __TVoid, typename TAttrParser,
			typename ... TAttrParserTail>
	Attr* _parse(u2 nameIndex, u4 len, const u1* data, const String& attrName,
			ClassFile& cp, void* args) {
		if (attrName == TAttrParser::AttrName) {
			BufferReader br(data, len);
			return TAttrParser().parse(br, cp, nameIndex, args);
		} else {
			return _parse<__TVoid, TAttrParserTail...>(nameIndex, len, data, attrName, cp, args);
		}
	}

};

}

#endif
