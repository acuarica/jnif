#ifndef JNIF_ATTRSPARSER_HPP
#define JNIF_ATTRSPARSER_HPP

#include "../base.hpp"

namespace jnif {

/**
 *
 */
template<typename ... TAttrParserList>
class AttrsParser {
public:

	template<typename TAttrVisitor>
	inline static void parse(BufferReader& br, ConstPool&cp, TAttrVisitor& av) {
		u2 attrCount = br.readu2();

		for (int i = 0; i < attrCount; i++) {
			u2 nameIndex = br.readu2();
			u4 len = br.readu4();
			const u1* data = br.pos();

			string attrName = cp.getUtf8(nameIndex);

			parse2<TAttrVisitor, TAttrParserList...>(nameIndex, len, data,
					attrName, cp, av);

			br.skip(len);
		}
	}

private:

	template<typename TAttrVisitor, typename TAttrParser,
			typename ... TAttrParserTail>
	inline static void parse2(u2 nameIndex, u4 len, const u1* data,
			const string& attrName, ConstPool& cp, TAttrVisitor& av) {
		if (attrName == TAttrParser::AttrName) {
			TAttrParser parser;
			BufferReader br(data, len);
			parser.parse(br, av, cp, nameIndex);
		} else {
			parse2<TAttrVisitor, TAttrParserTail...>(nameIndex, len, data,
					attrName, cp, av);
		}
	}

	template<typename TAttrVisitor>
	inline static void parse2(u2 nameIndex, u4 len, const u1* data,
			const string& attrName, ConstPool& cp, TAttrVisitor& av) {
		av.visitAttr(nameIndex, len, data);
	}
};

}

#endif
