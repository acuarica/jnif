#ifndef JNIF_LVTATTRPARSER_HPP
#define JNIF_LVTATTRPARSER_HPP

#include "../base.hpp"
#include "../tree/ConstPool.hpp"

namespace jnif {

/**
 *
 * Local variable table parser
 */
struct LvtAttrParser {

	static constexpr const char* AttrName = "LocalVariableTable";

	template<typename TReader, typename TVisitor>
	inline void parse(TReader& br, TVisitor& av, ConstPool& cp, u2 nameIndex) {
		u2 lvtlen = br.readu2();

		av.visitLvtCount(lvtlen);

		for (u2 i = 0; i < lvtlen; i++) {
			u2 startPc = br.readu2();
			u2 len = br.readu2();
			u2 varNameIndex = br.readu2();
			u2 varDescIndex = br.readu2();
			u2 index = br.readu2();

			av.visitLvtEntry(startPc, len, varNameIndex, varDescIndex, index);
		}
	}

	template<typename TWriter>
	struct Writer {
		TWriter& w;

		inline Writer(TWriter& w) :
				w(w) {
		}

		inline void visitLvtCount(u2 count) {
			w.writeu2(count);
		}

		inline void visitLvtEntry(u2 startPc, u2 len, u2 varNameIndex,
				u2 varDescIndex, u2 index) {
			w.writeu2(startPc);
			w.writeu2(len);
			w.writeu2(varNameIndex);
			w.writeu2(varDescIndex);
			w.writeu2(index);
		}
	};

	template<typename TVisitor>
	struct Forward {
		TVisitor av;

		inline Forward(TVisitor& av) :
				av(av) {
		}

		inline void visitLvtCount(u2 count) {
			av.visitLvtCount(count);
		}

		inline void visitLvtEntry(u2 startPc, u2 len, u2 varNameIndex,
				u2 varDescIndex, u2 index) {
			av.visitLvtEntry(startPc, len, varNameIndex, varDescIndex, index);
		}
	};
};

}

#endif
