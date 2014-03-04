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

	template<typename TWriter, typename _T = void>
	struct Writer: WriterBase<TWriter, TAttrParserList...> {

		TWriter& w;

		inline Writer(TWriter& w) :
				WriterBase<TWriter, TAttrParserList...>(w), w(w) {
		}

		inline void visitAttrCount(u2 attrCount) {
			w.writeu2(attrCount);
		}

		inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			w.writecount(data, len);
		}

		inline void visitAttrHeader(u2 nameIndex, u4 len) {
			w.writeu2(nameIndex);
			w.writeu4(len);
		}

		inline void visitAttrEnd() {
		}
	};

	template<typename _T>
	struct Writer<BufferWriter, _T> : WriterBase<BufferWriter,
			TAttrParserList...> {

		BufferWriter& w;

		u1* patch;
		int offset;

		inline Writer(BufferWriter& w) :
				WriterBase<BufferWriter, TAttrParserList...>(w), w(w), patch(0), offset(
						0) {
		}

		inline void visitAttrCount(u2 attrCount) {
			w.writeu2(attrCount);
		}

		inline void visitAttrHeader(u2 nameIndex, u4 len) {
			w.writeu2(nameIndex);

			patch = w.pos();
			w.skip(4);

			offset = w.offset2();

//			fprintf(stderr, "%d, %d - %p, %d\n", nameIndex, len, (void*) patch,
//					offset);
		}

		inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			w.writecount(data, len);
		}

		inline void visitAttrEnd() {
			int size = w.offset2() - offset;

			//fprintf(stderr, "  :: %p, %d, %d\n", (void*) patch, offset, size);

			BufferWriter bw(patch, 4);
			bw.writeu4(size);
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
				TAttrParser::template Forward<TVisitor>(w), ForwardBase<
						TVisitor, TAttrParserTail...>(w) {
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

		inline void visitAttrEnd() {
			v.visitAttrEnd();
		}
	};

private:

	template<typename TReader, typename TAttrVisitor, typename TAttrParser,
			typename ... TAttrParserTail>
	inline static void parseBase(u2 nameIndex, u4 len, const u1* data,
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
	inline static void parseBase(u2 nameIndex, u4 len, const u1* data,
			const std::string& attrName, ConstPool& cp, TAttrVisitor& av) {
		av.visitAttr(nameIndex, len, data);
	}
};

}

#endif
