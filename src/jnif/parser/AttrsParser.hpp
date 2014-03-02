#ifndef JNIF_ATTRSPARSER_HPP
#define JNIF_ATTRSPARSER_HPP

#include "../base.hpp"
#include "../tree/ConstPool.hpp"

namespace jnif {

/**
 *
 */
template<typename ... TAttrParserList>
class AttrsParser {
public:

	template<typename TAttrVisitor, typename TReader>
	inline static void parse(TReader& br, ConstPool&cp, TAttrVisitor& av) {
		u2 attrCount = br.readu2();

		av.visitAttrCount(attrCount);

		for (int i = 0; i < attrCount; i++) {
			u2 nameIndex = br.readu2();
			u4 len = br.readu4();

			av.visitAttrHeader(nameIndex, len);

			const u1* data = br.pos();

			std::string attrName = cp.getUtf8(nameIndex);

			parse2<TReader, TAttrVisitor, TAttrParserList...>(nameIndex, len,
					data, attrName, cp, av);

			br.skip(len);
		}
	}

	template<typename T, typename ... TAttrParserList2>
	class Writer2;

	template<typename TWriter>
	class Writer2<TWriter> {
	public:
		inline Writer2(TWriter& w) {
		}
	};

	template<typename TWriter, typename TAttrParser,
			typename ... TAttrParserTail>
	class Writer2<TWriter, TAttrParser, TAttrParserTail...> : public TAttrParser::template Writer<
			TWriter>,
			public Writer2<TWriter, TAttrParserTail...> {

	public:

		inline Writer2(TWriter& w) :
				TAttrParser::template Writer<TWriter>(w), Writer2<TWriter,
						TAttrParserTail...>(w)

		{
		}

	};

	template<typename TWriter>
	class Writer: public Writer2<TWriter, TAttrParserList...> {
	public:
		inline Writer(TWriter& w) :
				Writer2<TWriter, TAttrParserList...>(w), w(w) {
		}

		inline void visitAttrCount(u2 attrCount) {
			w.writeu2(attrCount);
		}

		inline void visitAttrHeader(u2 nameIndex, u4 len) {
			w.writeu2(nameIndex);
			w.writeu4(len);
		}

		inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			w.writecount(data, len);
		}

	private:
		TWriter& w;
	};

private:

	template<typename TReader, typename TAttrVisitor, typename TAttrParser,
			typename ... TAttrParserTail>
	inline static void parse2(u2 nameIndex, u4 len, const u1* data,
			const std::string& attrName, ConstPool& cp, TAttrVisitor& av) {
		if (attrName == TAttrParser::AttrName) {
			TAttrParser parser;
			TReader br(data, len);
			parser.parse(br, av, cp, nameIndex);
		} else {
			parse2<TReader, TAttrVisitor, TAttrParserTail...>(nameIndex, len,
					data, attrName, cp, av);
		}
	}

	template<typename TReader, typename TAttrVisitor>
	inline static void parse2(u2 nameIndex, u4 len, const u1* data,
			const std::string& attrName, ConstPool& cp, TAttrVisitor& av) {
		av.visitAttr(nameIndex, len, data);
	}
};

}

#endif
