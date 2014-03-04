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

			parseBase<TReader, TAttrVisitor, TAttrParserList...>(nameIndex, len,
					data, attrName, cp, av);

			br.skip(len);

			av.visitAttrEnd();
		}
	}

	template<typename TWriter, typename ... TAttrParserList2>
	struct _WriterBase;

	template<typename TWriter>
	struct _WriterBase<TWriter> {
		_WriterBase(TWriter& w) {
		}
	};

	template<typename TWriter, typename TAttrParser,
			typename ... TAttrParserTail>
	struct _WriterBase<TWriter, TAttrParser, TAttrParserTail...> : TAttrParser::template Writer<
			TWriter>,
			_WriterBase<TWriter, TAttrParserTail...> {

		_WriterBase(TWriter& w) :
				TAttrParser::template Writer<TWriter>(w), _WriterBase<TWriter,
						TAttrParserTail...>(w) {
		}
	};

	template<typename TWriter, typename _T = void>
	struct Writer: _WriterBase<TWriter, TAttrParserList...> {

		TWriter& w;

		Writer(TWriter& w) :
				_WriterBase<TWriter, TAttrParserList...>(w), w(w) {
		}

		void visitAttrCount(u2 attrCount) {
			w.writeu2(attrCount);
		}

		void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			w.writecount(data, len);
		}

		void visitAttrHeader(u2 nameIndex, u4 len) {
			w.writeu2(nameIndex);
			w.writeu4(len);
		}

		void visitAttrEnd() {
		}
	};

	template<typename TVisitor, typename ... TAttrParserList2>
	struct _ForwardBase;

	template<typename TVisitor>
	struct _ForwardBase<TVisitor> {
		_ForwardBase(TVisitor& w) {
		}
	};

	template<typename TVisitor, typename TAttrParser,
			typename ... TAttrParserTail>
	struct _ForwardBase<TVisitor, TAttrParser, TAttrParserTail...> : TAttrParser::template Forward<
			TVisitor>,
			_ForwardBase<TVisitor, TAttrParserTail...> {
		_ForwardBase(TVisitor& w) :
				TAttrParser::template Forward<TVisitor>(w), _ForwardBase<
						TVisitor, TAttrParserTail...>(w) {
		}
	};

	template<typename TVisitor>
	struct Forward: _ForwardBase<TVisitor, TAttrParserList...> {
		TVisitor& v;

		Forward(TVisitor& v) :
				_ForwardBase<TVisitor, TAttrParserList...>(v), v(v) {
		}

		void visitAttrCount(u2 attrCount) {
			v.visitAttrCount(attrCount);
		}

		void visitAttrHeader(u2 nameIndex, u4 len) {
			v.visitAttrHeader(nameIndex, len);
		}

		void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			v.visitAttr(nameIndex, len, data);
		}

		void visitAttrEnd() {
			v.visitAttrEnd();
		}
	};

private:

	template<typename TReader, typename TAttrVisitor, typename TAttrParser,
			typename ... TAttrParserTail>
	static void parseBase(u2 nameIndex, u4 len, const u1* data,
			const std::string& attrName, ConstPool& cp, TAttrVisitor& av) {
		if (attrName == TAttrParser::AttrName) {
			TAttrParser parser;
			TReader br(data, len);
			parser.parse(br, av, cp, nameIndex);
		} else {
			parseBase<TReader, TAttrVisitor, TAttrParserTail...>(nameIndex, len,
					data, attrName, cp, av);
		}
	}

	template<typename TReader, typename TAttrVisitor>
	static void parseBase(u2 nameIndex, u4 len, const u1* data,
			const std::string& attrName, ConstPool& cp, TAttrVisitor& av) {
		av.visitAttr(nameIndex, len, data);
	}
};

}

#endif
