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

	template<typename TWriter, typename ... TAttrParserList2>
	struct WriterBase;

	template<typename TWriter>
	struct WriterBase<TWriter> {
		inline WriterBase(TWriter& w) {
		}
	};

	template<typename TWriter, typename TAttrParser,
			typename ... TAttrParserTail>
	struct WriterBase<TWriter, TAttrParser, TAttrParserTail...> : TAttrParser::template Writer<
			TWriter>,
			WriterBase<TWriter, TAttrParserTail...> {

		inline WriterBase(TWriter& w) :
				TAttrParser::template Writer<TWriter>(w), WriterBase<TWriter,
						TAttrParserTail...>(w) {
		}
	};

	template<typename TWriter>
	struct Writer: WriterBase<TWriter, TAttrParserList...> {

		TWriter& w;

		inline Writer(TWriter& w) :
				WriterBase<TWriter, TAttrParserList...>(w), w(w) {
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
	};

	template<typename TVisitor, typename ... TAttrParserList2>
	struct ForwardBase;

	template<typename TVisitor>
	struct ForwardBase<TVisitor> {
		inline ForwardBase(TVisitor& w) {
		}
	};

	template<typename TVisitor, typename TAttrParser,
			typename ... TAttrParserTail>
	struct ForwardBase<TVisitor, TAttrParser, TAttrParserTail...> : TAttrParser::template Forward<
			TVisitor>,
			ForwardBase<TVisitor, TAttrParserTail...> {

		inline ForwardBase(TVisitor& w) :
				TAttrParser::template Forward<TVisitor>(w), ForwardBase<TVisitor,
						TAttrParserTail...>(w) {
		}
	};

	template<typename TVisitor>
	struct Forward: ForwardBase<TVisitor, TAttrParserList...> {
		TVisitor& v;

		inline Forward(TVisitor& v) :
				ForwardBase<TVisitor, TAttrParserList...>(v), v(v) {
		}

		inline void visitAttrCount(u2 attrCount) {
			v.visitAttrCount(attrCount);
		}

		inline void visitAttrHeader(u2 nameIndex, u4 len) {
			v.visitAttrHeader(nameIndex, len);
		}

		inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			v.visitAttr(nameIndex, len, data);
		}
	};

private:

	template<typename TReader, typename TAttrVisitor, typename TAttrParser,
			typename ... TAttrParserTail>
	inline static void parse2(u2 nameIndex, u4 len, const u1* data,
			const std::string& attrName, ConstPool& cp, TAttrVisitor& av) {
		if (attrName == TAttrParser::AttrName) {

			//av.TAttrParser::AttrName();

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
