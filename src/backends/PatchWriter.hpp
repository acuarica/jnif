/*
 * PatchWriter.cpp
 *
 *  Created on: Mar 4, 2014
 *      Author: luigi
 */

namespace jnif {

template<typename ...TAttrParserList>
struct CodeAttrPatchWriter {

	struct PatchCodeWriter: CodeAttrParser<TAttrParserList...>::template Writer<
			BufferWriter> {
		PatchCodeWriter(BufferWriter& w) :
				CodeAttrParser<TAttrParserList...>::template Writer<BufferWriter>(
						w), codeStartsPatch(0), offset(0) {
		}

		u1* codeStartsPatch;
		int offset;

		void beginCode(u4 codeLen) {
			codeStartsPatch = this->w.pos();

			this->w.skip(4);

			offset = this->w.offset2();
		}

		void codeEnd() {
			int size = this->w.offset2() - offset;

			BufferWriter bw(codeStartsPatch, 4);
			bw.writeu4(size);
		}
	};
};

template<typename ...TAttrParserList>
struct PatchAttrsWriter: AttrsParser<TAttrParserList...>::template Writer<
		BufferWriter> {

	BufferWriter& w;
	u1* patch;
	int offset;

	inline PatchAttrsWriter(BufferWriter& w) :
			AttrsParser<TAttrParserList...>::template Writer<BufferWriter>(w), w(
					w), patch(0), offset(0) {
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

struct PatchWriter: ClassParser::ClassWriter<BufferWriter> {
	PatchWriter(BufferWriter& w) :
			ClassParser::ClassWriter<BufferWriter>(w) {

	}
};

}
